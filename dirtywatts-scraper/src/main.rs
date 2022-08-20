#[macro_use]
extern crate diesel;
#[macro_use]
extern crate diesel_migrations;

use diesel::PgConnection;
use std::time::Duration;

use crate::db::{add_emi_stats, add_readings, create_connection};
use crate::{emi_stats::get_emi_stats, power_station::get_current_power};

mod db;
mod emi_stats;
mod power_station;

embed_migrations!();

async fn run_power_sources(conn: &PgConnection) {
    loop {
        let current_power = get_current_power().await;
        match current_power {
            Ok(val) => {
                match add_readings(val, conn) {
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

async fn run_emi_stats(conn: &PgConnection) {
    loop {
        let current_power = get_emi_stats().await;
        match current_power {
            Ok(val) => {
                add_emi_stats(val, conn);
            }
            Err(ex) => {
                println!("Error fetching data {ex}");
            }
        }

        tokio::time::sleep(Duration::from_secs(60 * 15)).await;
    }
}

#[tokio::main]
async fn main() -> anyhow::Result<()> {
    dotenv::dotenv().ok();
    println!("Creating connection");
    let conn = create_connection()?;
    println!("Created connection");

    println!("Running migrations");
    embedded_migrations::run(&conn)?;
    println!("Done!");

    tokio::select! {
        _ = run_power_sources(&conn) => {}
        _ = run_emi_stats(&conn) => {}
    }

    Ok(())
}
