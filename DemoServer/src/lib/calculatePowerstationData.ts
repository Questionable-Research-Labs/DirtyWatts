import type { PowerTypesResponse } from "./powerstation";

export function generateFakeData(slider: number): PowerTypesResponse {
    let scale = (min: number,max: number, start: number = 0) => {
        let virtualSliderPos = (Math.max((slider/100)-start,0))/(1-start)
        return Math.floor(virtualSliderPos * (max - min + 1)) + min
    };

    return {
        "timestamp": (new Date()).toISOString(),
        "power_types": {
            "battery": {
                "generation_mw": 0,
                "capacity_mw": 1
            },
            "co_gen": {
                "generation_mw": 117,
                "capacity_mw": 216
            },
            "coal": {
                "generation_mw": scale(0, 300, 0.5),
                "capacity_mw": 750
            },
            "gas": {
                "generation_mw": scale(0,600),
                "capacity_mw": 1280
            },
            "geothermal": {
                "generation_mw": 938,
                "capacity_mw": 1062
            },
            "hydro": {
                "generation_mw": 4000,
                "capacity_mw": 5415
            },
            "diesel": {
                "generation_mw": scale(0, 50, 0.5),
                "capacity_mw": 156
            },
            "wind": {
                "generation_mw": 438,
                "capacity_mw": 1040
            }
        }
    }
}