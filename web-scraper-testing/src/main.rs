use std::error::Error;

use chrono::{Utc, Local, DateTime, NaiveDateTime, TimeZone};
use serde_json::{Value};
use serde::{Serialize, Deserialize};
use std::result::Result;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>>{
    println!("{:?}",get_current_power().await?);
    Ok(())
}

async fn get_current_power()-> Result<PowerstationUpdatePackage, Box<dyn std::error::Error>>  {
    
    let api_url = format!("https://www.transpower.co.nz/aws/data/current_generation/{}",Local::now().timestamp());
    
    let transpower_api_raw = reqwest::get(api_url)
        .await?
        .text()
        .await?;
    let data: ApiJson = serde_json::from_str(&transpower_api_raw)?;
    let update: PowerstationUpdatePackage = PowerstationUpdatePackage {
        timestamp: Local.from_utc_datetime(&NaiveDateTime::from_timestamp(data.timestamp, 0)),
        power_types: data.data.new_zealand,
    };
    return Ok(update);
}

#[derive(Serialize, Deserialize, Debug)]
struct PowerStationType {
    #[serde(alias = "generation")]
    generation_mw: f64,
    #[serde(alias = "capacity")]
    capacity_mw: f64,
}


#[derive(Serialize, Deserialize, Debug)]
struct PowerTypes {
    #[serde(alias = "Battery")]
    battery: PowerStationType,
    #[serde(alias = "Co-Gen")]
    co_gen: PowerStationType,
    #[serde(alias = "Coal")]
    coal: PowerStationType,
    #[serde(alias = "Gas")]
    gas: PowerStationType,
    #[serde(alias = "Geothermal")]
    geothermal: PowerStationType,
    #[serde(alias = "Hydro")]
    hydro: PowerStationType,
    #[serde(alias = "Liquid")]
    liquid: PowerStationType,
    #[serde(alias = "Wind")]
    wind: PowerStationType,
}

#[derive(Debug)]
struct PowerstationUpdatePackage {
    timestamp: DateTime<Local>,
    power_types: PowerTypes,
}

#[derive(Serialize, Deserialize, Debug)]
struct CountryPower {
    #[serde(alias = "New Zealand")]
    new_zealand: PowerTypes,
}

#[derive(Serialize, Deserialize, Debug)]
struct ApiJson {
    data: CountryPower,
    date: String,
    timestamp: i64,
    update: bool
}