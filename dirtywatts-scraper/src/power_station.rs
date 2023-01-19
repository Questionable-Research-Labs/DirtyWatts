use chrono::{DateTime, Local, NaiveDateTime, TimeZone, Utc};
use influxdb2::models::WriteDataPoint;
use serde::{Deserialize, Serialize};

pub async fn get_current_power() -> Result<PowerStationUpdatePackage, Box<dyn std::error::Error>> {
    let api_url = format!(
        "https://www.transpower.co.nz/em6/data/current_generation/{}",
        Local::now().timestamp()
    );

    let transpower_api_raw = reqwest::get(api_url).await?.text().await?;
    let mut data: ApiJson = serde_json::from_str(&transpower_api_raw)?;

    let update: PowerStationUpdatePackage = PowerStationUpdatePackage {
        timestamp: Local
            .from_utc_datetime(&NaiveDateTime::from_timestamp(data.timestamp, 0))
            .with_timezone(&Utc),
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
    pub timestamp: DateTime<Utc>,
    pub power_types: PowerTypes,
}

impl PowerStationUpdatePackage {
    pub fn into_array(self) -> [PowerTypeUpdate; 8] {
        [
            PowerTypeUpdate {
                name: "battery".into(),
                capacity_mw: self.power_types.battery.capacity_mw,
                generation_mw: self.power_types.battery.generation_mw,
                timestamp: self.timestamp.timestamp_nanos(),
            },
            PowerTypeUpdate {
                name: "co-gen".into(),
                capacity_mw: self.power_types.co_gen.capacity_mw,
                generation_mw: self.power_types.co_gen.generation_mw,
                timestamp: self.timestamp.timestamp_nanos(),
            },
            PowerTypeUpdate {
                name: "coal".into(),
                capacity_mw: self.power_types.coal.capacity_mw,
                generation_mw: self.power_types.coal.generation_mw,
                timestamp: self.timestamp.timestamp_nanos(),
            },
            PowerTypeUpdate {
                name: "gas".into(),
                capacity_mw: self.power_types.gas.capacity_mw,
                generation_mw: self.power_types.gas.generation_mw,
                timestamp: self.timestamp.timestamp_nanos(),
            },
            PowerTypeUpdate {
                name: "geothermal".into(),
                capacity_mw: self.power_types.geothermal.capacity_mw,
                generation_mw: self.power_types.geothermal.generation_mw,
                timestamp: self.timestamp.timestamp_nanos(),
            },
            PowerTypeUpdate {
                name: "hydro".into(),
                capacity_mw: self.power_types.hydro.capacity_mw,
                generation_mw: self.power_types.hydro.generation_mw,
                timestamp: self.timestamp.timestamp_nanos(),
            },
            PowerTypeUpdate {
                name: "diesel".into(),
                capacity_mw: self.power_types.diesel.capacity_mw,
                generation_mw: self.power_types.diesel.generation_mw,
                timestamp: self.timestamp.timestamp_nanos(),
            },
            PowerTypeUpdate {
                name: "wind".into(),
                capacity_mw: self.power_types.wind.capacity_mw,
                generation_mw: self.power_types.wind.generation_mw,
                timestamp: self.timestamp.timestamp_nanos(),
            },
        ]
    }
}

pub struct PowerTypeUpdate {
    name: String,
    generation_mw: f64,
    capacity_mw: f64,
    timestamp: i64,
}

impl WriteDataPoint for PowerTypeUpdate {
    fn write_data_point_to<W>(&self, mut w: W) -> std::io::Result<()>
    where
        W: std::io::Write,
    {
        writeln!(
            w,
            "power_station,type={} generation_mw={},capacity_mw={} {}",
            self.name, self.generation_mw, self.capacity_mw, self.timestamp
        )
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
