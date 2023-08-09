import type { PowerStationsResponse, PowerTypes } from "./api"
import type { RGBColor } from "./utils"

export function calculateLightColourOld(power_types: PowerTypes): RGBColor {
    // UPDATE LIGHT
    let red_value = 0
    let green_value = 0

    if (power_types.coal.generation_mw > 0 || power_types.diesel.generation_mw > 0) {
        let half_max_coal = Math.round(power_types.coal.capacity_mw / 2 + power_types.diesel.capacity_mw / 2)
        let bad_generation = (power_types.coal.generation_mw + power_types.diesel.generation_mw) / half_max_coal
        if (bad_generation > 1) {
            bad_generation = 1
        }

        red_value = 150 + Math.round(105 * bad_generation)
        green_value = Math.round(95 - (95 * bad_generation))

    } else {
        let half_max_gas = Math.round(power_types.gas.capacity_mw / 2)
        let medium_generation = power_types.gas.generation_mw / half_max_gas
        if (medium_generation > 1) {
            medium_generation = 1
        }

        red_value = Math.round(160 * medium_generation)
        green_value = Math.round(255 - (127 * medium_generation))
    }

    return [red_value,green_value,0]

}

const COLOUR_MAP = [[24, 219, 0], [73, 216, 0], [99, 212, 0], [119, 208, 0], [136, 204, 0], [153, 200, 0], [169, 195, 0], [183, 190, 0], [195, 185, 0], [208, 179, 0], [219, 174, 0], [230, 167, 0], [243, 159, 0], [255, 150, 19], [255, 139, 31], [255, 126, 40], [255, 112, 47], [255, 94, 53], [255, 71, 58], [255, 28, 63]]
const CO2_INTENSITY_RANGE = [24, 159]
const CO2_OUTPUT_RANGE = [100, 975]

export function calculateLightColour(data: PowerStationsResponse): RGBColor {
    let co2_output = Math.max(Math.min(data.co2e_tonnne_per_hour, CO2_OUTPUT_RANGE[1]), CO2_OUTPUT_RANGE[0])

    let co2_output_percent = (co2_output - CO2_OUTPUT_RANGE[0]) / (CO2_OUTPUT_RANGE[1] - CO2_OUTPUT_RANGE[0])

    let index = Math.round(co2_output_percent * (COLOUR_MAP.length - 1))
    return COLOUR_MAP[index] as RGBColor
}