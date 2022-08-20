const API_URL = "https://dirty-watts-api.host.qrl.nz"

export type PowerTypes = Record<string, PowerType>

export interface PowerStationsResponse {
    timestamp: string;
    power_types: PowerTypes;
}

export interface PowerType {
    generation_mw: number;
    capacity_mw: number;
}

async function fetchAPI<T>(path: string): Promise<T> {
    return await fetch(`${API_URL}/${path}`)
        .then((res) => res.json())
}

export async function getPowerStations(): Promise<PowerStationsResponse> {
    return fetchAPI<PowerStationsResponse>("live/power_stations")
}

export async function getPowerStationsHistory(): Promise<PowerStationsResponse[]> {
    return fetchAPI<PowerStationsResponse[]>("history/power_stations")
}