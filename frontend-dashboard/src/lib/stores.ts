import { derived, writable, type Writable } from "svelte/store";
import type { ConnectionPoint, PowerStationsResponse, PowerType } from "./api";

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
    solar: true
};

export const percentRenewable = derived(powerTypes, (powerTypes) => {
    if (!powerTypes) {
        return null;
    }
    let total_clean = 0;
    let total_dirty = 0;
    for (const key in powerTypes?.power_types) {
        const { generation_mw }: PowerType = powerTypes?.power_types[key];
        if (cleanPowerIndex[key]) {
            total_clean += generation_mw;
        } else {
            total_dirty += generation_mw;
        }
    }
    let output = total_clean / (total_clean + total_dirty);
    return output;
});
