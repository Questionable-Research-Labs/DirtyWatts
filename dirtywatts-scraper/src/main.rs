use diesel_migrations::{embed_migrations, EmbeddedMigrations};

use diesel::PgConnection;
use std::time::Duration;
use diesel_migrations::MigrationHarness;

use crate::db::{add_emi_stats, add_readings, create_connection};
use crate::{emi_stats::get_emi_stats, power_station::get_current_power};

mod db;
mod emi_stats;
mod power_station;

pub const MIGRATIONS: EmbeddedMigrations = embed_migrations!();

async fn run_power_sources(conn: &mut PgConnection) {
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

async fn run_emi_stats(conn: &mut PgConnection) {
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
    if dotenv::dotenv().is_err() {
        println!("No .env file found");
    }

    println!("Creating connections");
    let mut conn_a = create_connection()?;
    let mut conn_b = create_connection()?;

    println!("Running migrations");
    conn_a.run_pending_migrations(MIGRATIONS).unwrap();
    println!("Done!");

    tokio::select! {
        _ = run_power_sources(&mut conn_a) => {}
        _ = run_emi_stats(&mut conn_b) => {}
    }

    Ok(())
}
