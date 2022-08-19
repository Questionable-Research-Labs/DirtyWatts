use std::fs;

use chrono::{DateTime, Local, NaiveDateTime, TimeZone};
use serde::{Deserialize, Serialize};
use serde_json;

pub async fn get_emi_stats() -> Result<Vec<ConnectionPoint>, Box<dyn std::error::Error>> {
    let raw_data = fs::read_to_string("./api_defs/emiResponse.json")?;
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

// "PointOfConnectionCode": "ABY0111",
// "FiveMinuteIntervalDatetime": "2020-10-31T17:00:00",
// "FiveMinuteIntervalNumber": 1,
// "RunDateTime": "2020-10-31T03:59:01",
// "SPDLoadMegawatt": 2.613,
// "SPDGenerationMegawatt": 0.000,
// "DollarsPerMegawattHour": 112.00

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
