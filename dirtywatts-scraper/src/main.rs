use chrono::{NaiveDate, NaiveDateTime};
use db::InfluxConfig;
use influxdb2::api::buckets::ListBucketsRequest;
use influxdb2::api::organization::ListOrganizationRequest;
use influxdb2::models::{DataPoint, PostBucketRequest, WriteDataPoint};
use influxdb2::{Client, RequestError};
use reqwest::StatusCode;
use std::fs::File;
use std::time::Duration;

use crate::db::{add_emi_stats, add_readings};
use crate::{emi_stats::get_emi_stats, power_station::get_current_power};

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

        tokio::time::sleep(Duration::from_secs(30)).await;
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

    tokio::select! {
        // _ = run_power_sources(&client) => {}
        _ = run_emi_stats(&client) => {}
    }

    Ok(())
}
