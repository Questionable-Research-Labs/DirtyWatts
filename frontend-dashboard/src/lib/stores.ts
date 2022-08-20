import { writable, type Writable } from "svelte/store";
import type { ConnectionPoint, PowerStationsResponse, PowerType } from "./api";

export let powerTypes: Writable<PowerStationsResponse | null> = writable(null);
export let connectionPoints: Writable<ConnectionPoint[]> = writable([]);
export let powerTypesHistory: Writable<PowerStationsResponse[]> = writable([]);