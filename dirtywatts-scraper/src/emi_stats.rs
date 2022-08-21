use chrono::{DateTime, Local, NaiveDateTime, TimeZone, Utc};
use dotenv;
use serde::{Deserialize, Serialize};
use serde_json;

pub async fn get_emi_stats() -> Result<Vec<ConnectionPoint>, Box<dyn std::error::Error>> {
    let api_key = dotenv::var("EMI_API_KEY").unwrap();

    let api_url = "https://emi.azure-api.net/real-time-dispatch";
    let client = reqwest::Client::new();

    let raw_data = client
        .get(api_url)
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
            timestamp: Local
                .from_utc_datetime(
                    &NaiveDateTime::parse_from_str(&x.datetime, "%Y-%m-%dT%H:%M:%S")
                        .unwrap_or(NaiveDateTime::from_timestamp(0, 0)),
                )
                .with_timezone(&Utc),
            generation_mw: if x.load_mw < 0.0 { -x.load_mw } else { 0.0 } + x.generation_mw,
            load_mw: if x.load_mw < 0.0 { 0.0 } else { x.load_mw },
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
    run_datetime: Option<String>,
    #[serde(alias = "SPDLoadMegawatt")]
    load_mw: f64,
    #[serde(alias = "SPDGenerationMegawatt")]
    generation_mw: f64,
    #[serde(alias = "DollarsPerMegawattHour")]
    mwh_price: f64,
}

#[derive(Clone, Debug)]
pub struct ConnectionPoint {
    pub connection_code: String,
    pub timestamp: DateTime<Utc>,
    pub load_mw: f64,
    pub generation_mw: f64,
    pub mwh_price: f64,
}
