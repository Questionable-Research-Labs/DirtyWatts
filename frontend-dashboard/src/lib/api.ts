import { connectionPoints, powerTypes, powerTypesHistory } from "./stores";

const API_URL = "http://localhost:8000";

export type PowerTypes = Record<string, PowerType>;

export interface PowerStationsResponse {
  timestamp: string;
  power_types: PowerTypes;
}

export interface PowerType {
  generation_mw: number;
  capacity_mw: number;
}

export interface ConnectionPoint {
<<<<<<< Updated upstream
    connection_code: string,
    timestamp: string,
    load_mw: number,
    generation_mw: number,
    mwh_price: number,
    latitude: number,
    longitude: number,
    address: string
=======
  connection_code: string;
  timestamp: string;
  load_mw: number;
  generation_mw: number;
  mwh_price: number;
  latitude: number;
  longitude: number;
  network_region_id: number;
  network_region_name: string;
  network_region_zone: string;
  address: string;
>>>>>>> Stashed changes
}

async function fetchAPI<T>(path: string): Promise<T> {
  return await fetch(`${API_URL}/${path}`).then((res) => res.json());
}

export async function getPowerStations(): Promise<PowerStationsResponse> {
  return fetchAPI<PowerStationsResponse>("live/power_stations");
}

export async function getPowerStationsHistory(): Promise<
  PowerStationsResponse[]
> {
  return fetchAPI<PowerStationsResponse[]>("history/power_stations");
}

export async function getConnectionPoints(): Promise<ConnectionPoint[]> {
  return fetchAPI<ConnectionPoint[]>("live/grid_connection_points");
}
export async function getConnectionPointHistory(
  point_code: string
): Promise<ConnectionPoint[]> {
  return fetchAPI<ConnectionPoint[]>(
    `history/grid_connection_points/${point_code}`
  );
}

export async function initialiseAPI() {
  powerTypes.set(await getPowerStations());
  powerTypesHistory.set(await getPowerStationsHistory());
  connectionPoints.set(await getConnectionPoints());
}
