use std::fs;

use chrono::{DateTime, Local, NaiveDateTime, TimeZone};
use serde::{Deserialize, Serialize};
use serde_json;
use dotenv;

pub async fn get_emi_stats() -> Result<Vec<ConnectionPoint>, Box<dyn std::error::Error>> {
    dotenv::dotenv().ok();
    let api_key = dotenv::var("EMI_API_KEY").unwrap();
    
    let api_url = "https://emi.azure-api.net/real-time-dispatch";
    let client = reqwest::Client::new();

    
    let raw_data = client.get(api_url)
        .header("Ocp-Apim-Subscription-Key", api_key)
        .send()
        .await?
        .text()
        .await?;

    let data: Vec<ConnectionPointAPI> = serde_json::from_str(&raw_data)?;
    let formatted: Vec<ConnectionPoint> = data
        .iter()
        .map(|x| ConnectionPoint {
            connection_code: x.connection_code.clone(),
            timestamp: Local.from_utc_datetime(
                &NaiveDateTime::parse_from_str(&x.datetime, "%Y-%m-%dT%H:%M:%S")
                    .unwrap_or(NaiveDateTime::from_timestamp(0, 0)),
            ),
            generation_mw: x.generation_mw,
            load_mw: x.load_mw,
            mwh_price: x.mwh_price,
        })
        .collect();
    
    return Ok(formatted);
}

#[derive(Serialize, Deserialize, Debug)]
struct ConnectionPointAPI {
    #[serde(alias = "PointOfConnectionCode")]
    connection_code: String,
    #[serde(alias = "FiveMinuteIntervalDatetime")]
    datetime: String,
    #[serde(alias = "FiveMinuteIntervalNumber")]
    interval_number: u32,
    #[serde(alias = "RunDateTime")]
    run_datetime: String,
    #[serde(alias = "SPDLoadMegawatt")]
    load_mw: f64,
    #[serde(alias = "SPDGenerationMegawatt")]
    generation_mw: f64,
    #[serde(alias = "DollarsPerMegawattHour")]
    mwh_price: f64,
}

#[derive(Clone, Debug)]
pub struct ConnectionPoint {
    connection_code: String,
    timestamp: DateTime<Local>,
    load_mw: f64,
    generation_mw: f64,
    mwh_price: f64,
}
