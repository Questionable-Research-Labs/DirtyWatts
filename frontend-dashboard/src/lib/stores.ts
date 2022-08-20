import { writable, type Writable } from "svelte/store";
import type { ConnectionPoint, PowerStationsResponse } from "./api";

export const powerTypes: Writable<PowerStationsResponse | null> = writable(null);
export const connectionPoints: Writable<ConnectionPoint[]> = writable([]);
export const powerTypesHistory: Writable<PowerStationsResponse[]> = writable([]);