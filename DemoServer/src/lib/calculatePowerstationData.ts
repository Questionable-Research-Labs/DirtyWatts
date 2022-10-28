
export function generateFakeData(slider: number) {
    let scale = (min: number,max: number) => Math.floor((slider/100) * (max - min + 1)) + min;

    return {
        "timestamp": "2022-10-28T06:30:00+00:00",
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
                "generation_mw": scale(0, 150),
                "capacity_mw": 750
            },
            "gas": {
                "generation_mw": scale(20,500),
                "capacity_mw": 1280
            },
            "geothermal": {
                "generation_mw": 938,
                "capacity_mw": 1062
            },
            "hydro": {
                "generation_mw": scale(4000, 2000),
                "capacity_mw": 5415
            },
            "diesel": {
                "generation_mw": scale(0, 50),
                "capacity_mw": 156
            },
            "wind": {
                "generation_mw": 438,
                "capacity_mw": 1040
            }
        }
    }
}