use chrono::{FixedOffset, Local, NaiveDateTime, TimeZone};
use dirtywatts_common::{connection_points::get_index, ConnectionPoint};
use dotenv;
use serde::{Deserialize, Serialize};

pub async fn get_emi_stats() -> Result<Vec<ConnectionPoint>, Box<dyn std::error::Error>> {
    let api_key = dotenv::var("EMI_API_KEY").unwrap();

    let api_url = "https://emi.azure-api.net/real-time-dispatch";
    let client = reqwest::Client::new();

    let data: Vec<ConnectionPointAPI> = client
        .get(api_url)
        .header("Ocp-Apim-Subscription-Key", api_key)
        .send()
        .await?
        .json()
        .await?;

    let formatted: Vec<ConnectionPoint> = data
        .iter()
        .filter_map(|x| {
            get_index(x.connection_code.as_str()).map(|_| ConnectionPoint {
                connection_code: x.connection_code.clone(),
                time: Local
                    .from_local_datetime(
                        &NaiveDateTime::parse_from_str(&x.datetime, "%Y-%m-%dT%H:%M:%S")
                            .unwrap_or(NaiveDateTime::from_timestamp_opt(0, 0).unwrap()),
                    )
                    .unwrap()
                    .with_timezone(&FixedOffset::east_opt(0).unwrap()),
                generation_mw: if x.load_mw < 0.0 { -x.load_mw } else { 0.0 } + x.generation_mw,
                load_mw: if x.load_mw < 0.0 { 0.0 } else { x.load_mw },
                mwh_price: x.mwh_price,
            })
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
