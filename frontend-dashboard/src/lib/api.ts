import { connectionPoints, powerTypes, powerTypesHistory } from "./stores";

const API_URL = "https://api.dirtywatts.nz";
const OFFSET_HRS = 72;
const OFFSET = OFFSET_HRS * 3600 * 1000;

export type PowerTypes = Record<string, PowerType>;

export interface PowerStationsResponse {
  timestamp: string;
  power_types: PowerTypes;
  co2e_tonnne_per_hour: number;
  co2e_grams_per_kwh: number;
}

export interface PowerType {
  generation_mw: number;
  capacity_mw: number;
}

export interface ConnectionPoint {
  connection_code: string;
  timestamp: string;
  load_mw: number;
  generation_mw: number;
  mwh_price: number;
  latitude: number;
  longitude: number;
  address: string;
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
  
  const INTERVAL = 60;
  const INTERVAL_MS = INTERVAL * 60 * 1000;

  // Round the date to the nearest interval to make the API call easier to cache
  let startDate = new Date(Math.round((Date.now() - OFFSET)/INTERVAL_MS)*INTERVAL_MS);

  return fetchAPI<PowerStationsResponse[]>(
    `history/power_stations?start=${startDate.toISOString()}&time_interval_minutes=${INTERVAL}`
  );
}

export async function getConnectionPoints(): Promise<ConnectionPoint[]> {
  return fetchAPI<ConnectionPoint[]>("live/grid_connection_points");
}
export async function getConnectionPointHistory(
  point_code: string
): Promise<ConnectionPoint[]> {

  const INTERVAL = 30;
  const INTERVAL_MS = INTERVAL * 60 * 1000;

  // Round the date to the nearest interval to make the API call easier to cache
  let startDate = new Date(Math.round((Date.now() - OFFSET)/INTERVAL_MS)*INTERVAL_MS);

  return fetchAPI<ConnectionPoint[]>(
    `history/grid_connection_points/${point_code}?start=${startDate.toISOString()}&time_interval_minutes=${INTERVAL}`
  );
}

export async function initialiseAPI() {
  powerTypes.set(await getPowerStations());
  powerTypesHistory.set(await getPowerStationsHistory());
  connectionPoints.set(await getConnectionPoints());
}
