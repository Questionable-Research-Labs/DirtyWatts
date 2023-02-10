use chrono::{DateTime, Local, TimeZone, Utc, FixedOffset};
use dirtywatts_common::PowerReading;
use serde::{Deserialize, Serialize};

pub async fn get_current_power() -> Result<PowerStationUpdatePackage, Box<dyn std::error::Error>> {
    let api_url = format!(
        "https://www.transpower.co.nz/em6/data/current_generation/{}",
        Local::now().timestamp()
    );

    let data: ApiJson = reqwest::get(api_url).await?.json().await?;

    let update: PowerStationUpdatePackage = PowerStationUpdatePackage {
        timestamp: 
            Utc.timestamp_opt(data.timestamp, 0 ).unwrap().with_timezone(&FixedOffset::east_opt(0).unwrap()),
        power_types: data.data.new_zealand,
    };

    return Ok(update);
}

#[derive(Serialize, Deserialize, Debug)]
pub struct PowerStationType {
    #[serde(alias = "generation")]
    pub generation_mw: f64,
    #[serde(alias = "capacity")]
    pub capacity_mw: f64,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct PowerTypes {
    #[serde(alias = "Battery")]
    pub battery: PowerStationType,
    #[serde(alias = "Co-Gen")]
    pub co_gen: PowerStationType,
    #[serde(alias = "Coal")]
    pub coal: PowerStationType,
    #[serde(alias = "Gas")]
    pub gas: PowerStationType,
    #[serde(alias = "Geothermal")]
    pub geothermal: PowerStationType,
    #[serde(alias = "Hydro")]
    pub hydro: PowerStationType,
    #[serde(alias = "Liquid")]
    pub diesel: PowerStationType,
    #[serde(alias = "Wind")]
    pub wind: PowerStationType,
}

#[derive(Debug)]
pub struct PowerStationUpdatePackage {
    pub timestamp: DateTime<FixedOffset>,
    pub power_types: PowerTypes,
}

impl PowerStationUpdatePackage {
    pub fn into_array(self) -> [PowerReading; 8] {
        [
            PowerReading {
                name: "battery".into(),
                capacity_mw: self.power_types.battery.capacity_mw,
                generation_mw: self.power_types.battery.generation_mw,
                time: self.timestamp,
            },
            PowerReading {
                name: "co-gen".into(),
                capacity_mw: self.power_types.co_gen.capacity_mw,
                generation_mw: self.power_types.co_gen.generation_mw,
                time: self.timestamp,
            },
            PowerReading {

                name: "coal".into(),
                capacity_mw: self.power_types.coal.capacity_mw,
                generation_mw: self.power_types.coal.generation_mw,
                time: self.timestamp,
            },
            PowerReading {
                name: "gas".into(),
                capacity_mw: self.power_types.gas.capacity_mw,
                generation_mw: self.power_types.gas.generation_mw,
                time: self.timestamp,
            },
            PowerReading {
                name: "geothermal".into(),
                capacity_mw: self.power_types.geothermal.capacity_mw,
                generation_mw: self.power_types.geothermal.generation_mw,
                time: self.timestamp,
            },
            PowerReading {
                name: "hydro".into(),
                capacity_mw: self.power_types.hydro.capacity_mw,
                generation_mw: self.power_types.hydro.generation_mw,
                time: self.timestamp,
            },
            PowerReading {
                name: "diesel".into(),
                capacity_mw: self.power_types.diesel.capacity_mw,
                generation_mw: self.power_types.diesel.generation_mw,
                time: self.timestamp,
            },
            PowerReading {
                name: "wind".into(),
                capacity_mw: self.power_types.wind.capacity_mw,
                generation_mw: self.power_types.wind.generation_mw,
                time: self.timestamp,
            },
        ]
    }
}

#[derive(Serialize, Deserialize, Debug)]
struct CountryPower {
    #[serde(alias = "New Zealand")]
    pub new_zealand: PowerTypes,
}

#[derive(Serialize, Deserialize, Debug)]
struct ApiJson {
    pub data: CountryPower,
    pub date: String,
    pub timestamp: i64,
    pub update: bool,
}
