import { derived, writable, type Readable, type Writable } from "svelte/store";
import type { ConnectionPoint, PowerStationsResponse, PowerType } from "./api";
import { findClosest, type RGBColor } from "./utils";
import { calculateLightColour, calculateLightColourOld } from "./calculateLightColour";

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


export const previewPosition = writable(0.5);

export const previewDatapoint: Readable<PowerStationsResponse | null> = derived([powerTypesHistory, previewPosition], ([powerTypesHistory, previewPosition]) => findClosest(powerTypesHistory, previewPosition));

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



export const newLightColorSystem: Writable<boolean> = writable(true);

export const lightColour: Readable<RGBColor> = derived([previewDatapoint, newLightColorSystem], ([previewDatapoint, newLightColorSystem]) => {
    if (!previewDatapoint) {
        return [255,255,255] as RGBColor;
    }

    if (newLightColorSystem) {
        return calculateLightColour(previewDatapoint);
    } else {
        return calculateLightColourOld(previewDatapoint.power_types);
    }
});