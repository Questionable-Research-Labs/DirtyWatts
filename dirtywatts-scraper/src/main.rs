use crate::db::{add_emi_stats, add_readings};
use crate::{emi_stats::get_emi_stats, power_station::get_current_power};
use chrono::{NaiveDate, NaiveDateTime};
use db::InfluxConfig;
use influxdb2::Client;
use std::time::Duration;

mod db;
mod emi_stats;
mod power_station;

const BUCKET_NAME: &'static str = "Dirty Watts";

async fn run_power_sources(client: &Client) {
    loop {
        let current_power = get_current_power().await;
        match current_power {
            Ok(val) => {
                match add_readings(val, client).await {
                    Ok(_) => {}
                    Err(ex) => {
                        println!("Error uploading data: {ex}")
                    }
                };
            }
            Err(ex) => {
                println!("Error fetching data {ex}");
            }
        }

        tokio::time::sleep(Duration::from_secs(60)).await;
    }
}

async fn run_emi_stats(client: &Client) {
    loop {
        let current_power = get_emi_stats().await;
        match current_power {
            Ok(val) => {
                match add_emi_stats(val, client).await {
                    Ok(_) => {}
                    Err(ex) => {
                        eprintln!("Could not add emi stats {ex}")
                    }
                };
            }
            Err(ex) => {
                println!("Error fetching data {ex}");
            }
        }

        tokio::time::sleep(Duration::from_secs(1800)).await;
    }
}

#[tokio::main]
async fn main() -> anyhow::Result<()> {
    _ = dotenv::dotenv();

    let InfluxConfig {
        url,
        org,
        auth_token,
    } = envy::prefixed("INFLUXDB_")
        .from_env()
        .expect("Please set the variables INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_AUTH_TOKEN");

    let client = Client::new(url, org.clone(), auth_token);

    client
        .delete(
            "Dirty Watts",
            NaiveDate::from_ymd(2020, 1, 1).and_hms(0, 0, 0),
            NaiveDate::from_ymd(2024, 1, 1).and_hms(0, 0, 0),
            Some(r#"_measurement = "emi_stat""#.into()),
        )
        .await
        .unwrap();

    tokio::select! {
        // _ = run_power_sources(&client) => {}
        _ = run_emi_stats(&client) => {}
    }

    Ok(())
}
