// JSON
export interface PowerTypesResponse {
    timestamp: string,
    power_types: PowerTypes
}

export interface PowerTypes {
    battery: GenerationPoint
    co_gen: GenerationPoint
    coal: GenerationPoint
    gas: GenerationPoint
    geothermal: GenerationPoint
    hydro: GenerationPoint
    diesel: GenerationPoint
    wind: GenerationPoint
}
export interface GenerationPoint {
    generation_mw: number,
    capacity_mw: number
}