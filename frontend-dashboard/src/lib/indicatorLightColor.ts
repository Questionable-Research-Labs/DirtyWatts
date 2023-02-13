import type { PowerTypes } from "./api"

// Taine's famous indicator light color calculator
// Used to mimic the physical indicator lights in the UI
export function calculateLightColour(power_types: PowerTypes): [number, number, number] {
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
