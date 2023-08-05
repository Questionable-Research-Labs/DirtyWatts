import { derived, writable, type Readable, type Writable } from "svelte/store";
import type { ConnectionPoint, PowerStationsResponse, PowerType } from "./api";
import type { RGBColor } from "./utils";
import { calculateLightColourOld } from "./calculateLightColour";

export const powerTypes: Writable<PowerStationsResponse | null> = writable(null);
export const connectionPoints: Writable<ConnectionPoint[]> = writable([]);
export const powerTypesHistory: Writable<PowerStationsResponse[]> = writable([]);

const cleanPowerIndex: Record<string, boolean> = {
    battery: true,
    co_gen: false,
    coal: false,
    gas: false,
    geothermal: true,
    hydro: true,
    diesel: false,
    wind: true,
};


export const previewPosition = writable(0);
export const previewDatapoint: Readable<PowerStationsResponse | null> = derived([powerTypesHistory, previewPosition], ([powerTypesHistory, previewPosition]) => {
        if (!powerTypesHistory.length) {
            return null;
        }
        powerTypesHistory = powerTypesHistory.sort((a, b) => new Date(a.timestamp).getTime() - new Date(b.timestamp).getTime());
        console.log(powerTypesHistory, previewPosition);
        // Get min and max dates in data
        let minDate = new Date(powerTypesHistory[0].timestamp);
        let maxDate = new Date(powerTypesHistory[powerTypesHistory.length - 1].timestamp);
        // Get interpolated date
        let date = new Date(
            minDate.getTime() +
                (maxDate.getTime() - minDate.getTime()) * previewPosition
        );
        // Find nearest datapoints and interpolate between them
        let index = powerTypesHistory.findIndex(
            (d) => new Date(d.timestamp) > date
        );
        let prev = powerTypesHistory[index - 1];
        let next = powerTypesHistory[index];
        console.log(prev, next, date)
        if (!prev || !next) {
            return powerTypesHistory[powerTypesHistory.length - 1];
        }

        let prevDate = new Date(prev.timestamp);
        let nextDate = new Date(next.timestamp);
        let prevWeight = (date.getTime() - prevDate.getTime()) / (nextDate.getTime() - prevDate.getTime());
        let nextWeight = 1 - prevWeight;
        return {
            timestamp: date.toISOString(),
            power_types: {
                battery: {
                    generation_mw: prevWeight * prev.power_types.battery.generation_mw + nextWeight * next.power_types.battery.generation_mw,
                    capacity_mw: prevWeight * prev.power_types.battery.capacity_mw + nextWeight * next.power_types.battery.capacity_mw,
                },
                co_gen: {
                    generation_mw: prevWeight * prev.power_types.co_gen.generation_mw + nextWeight * next.power_types.co_gen.generation_mw,
                    capacity_mw: prevWeight * prev.power_types.co_gen.capacity_mw + nextWeight * next.power_types.co_gen.capacity_mw,
                },
                coal: {
                    generation_mw: prevWeight * prev.power_types.coal.generation_mw + nextWeight * next.power_types.coal.generation_mw,
                    capacity_mw: prevWeight * prev.power_types.coal.capacity_mw + nextWeight * next.power_types.coal.capacity_mw,
                },
                gas: {
                    generation_mw: prevWeight * prev.power_types.gas.generation_mw + nextWeight * next.power_types.gas.generation_mw,
                    capacity_mw: prevWeight * prev.power_types.gas.capacity_mw + nextWeight * next.power_types.gas.capacity_mw,
                },
                geothermal: {
                    generation_mw: prevWeight * prev.power_types.geothermal.generation_mw + nextWeight * next.power_types.geothermal.generation_mw,
                    capacity_mw: prevWeight * prev.power_types.geothermal.capacity_mw + nextWeight * next.power_types.geothermal.capacity_mw,
                },
                hydro: {
                    generation_mw: prevWeight * prev.power_types.hydro.generation_mw + nextWeight * next.power_types.hydro.generation_mw,
                    capacity_mw: prevWeight * prev.power_types.hydro.capacity_mw + nextWeight * next.power_types.hydro.capacity_mw,
                },
                diesel: {
                    generation_mw: prevWeight * prev.power_types.diesel.generation_mw + nextWeight * next.power_types.diesel.generation_mw,
                    capacity_mw: prevWeight * prev.power_types.diesel.capacity_mw + nextWeight * next.power_types.diesel.capacity_mw,
                },
                wind: {
                    generation_mw: prevWeight * prev.power_types.wind.generation_mw + nextWeight * next.power_types.wind.generation_mw,
                    capacity_mw: prevWeight * prev.power_types.wind.capacity_mw + nextWeight * next.power_types.wind.capacity_mw,
                },
            },
        };
});

export const percentRenewable = derived(previewDatapoint, (previewDatapoint) => {
    if (!previewDatapoint) {
        return null;
    }
    let total_clean = 0;
    let total_dirty = 0;
    for (const key in previewDatapoint?.power_types) {
        const { generation_mw }: PowerType = previewDatapoint?.power_types[key];
        if (cleanPowerIndex[key]) {
            total_clean += generation_mw;
        } else {
            total_dirty += generation_mw;
        }
    }
    let output = total_clean / (total_clean + total_dirty);
    return output;
});



export const newLightColorSystem: Writable<boolean> = writable(false);

export const lightColour: Readable<RGBColor> = derived([previewDatapoint, newLightColorSystem], ([previewDatapoint, newLightColorSystem]) => {
    if (!previewDatapoint) {
        return [255,255,255] as RGBColor;
    }

    if (newLightColorSystem) {
        return [255,200,0] as RGBColor;
    } else {
        return calculateLightColourOld(previewDatapoint.power_types);
    }
});