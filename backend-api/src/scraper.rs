use chrono::{DateTime, Local};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

async fn get_current_power() -> anyhow::Result<ApiJson> {
    let api_url = format!(
        "https://www.transpower.co.nz/aws/data/current_generation/{}",
        Local::now().timestamp()
    );

    let transpower_api_raw = reqwest::get(api_url).await?.text().await?;

    let data: ApiJson = serde_json::from_str(&transpower_api_raw)?;

    return Ok(data);
}

#[derive(Serialize, Deserialize, Debug)]
struct GenerationReport {
    #[serde(alias = "generation")]
    generation_mw: f64,
    #[serde(alias = "capacity")]
    capacity_mw: f64,
}

#[derive(Serialize, Deserialize, Debug)]
struct ApiJson {
    data: HashMap<String, HashMap<String, GenerationReport>>,
    date: String,
    timestamp: i64,
    update: bool,
}
