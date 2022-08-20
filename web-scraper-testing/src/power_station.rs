use chrono::{DateTime, Local, NaiveDateTime, TimeZone, Utc};
use serde::{Deserialize, Serialize};

pub async fn get_current_power() -> Result<PowerStationUpdatePackage, Box<dyn std::error::Error>> {
    let api_url = format!(
        "https://www.transpower.co.nz/aws/data/current_generation/{}",
        Local::now().timestamp()
    );

    let transpower_api_raw = reqwest::get(api_url).await?.text().await?;
    let data: ApiJson = serde_json::from_str(&transpower_api_raw)?;
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
    pub liquid: PowerStationType,
    #[serde(alias = "Wind")]
    pub wind: PowerStationType,
}

#[derive(Debug)]
pub struct PowerStationUpdatePackage {
    pub timestamp: DateTime<Utc>,
    pub power_types: PowerTypes,
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
