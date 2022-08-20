use diesel::prelude::*;
use std::env;

pub use emi_stats::add_emi_stats;
pub use power_stations::add_readings;

mod emi_stats;
mod models;
mod power_stations;
mod schema;

pub fn create_connection() -> anyhow::Result<PgConnection> {
    let addr = env::var("DATABASE_URL")?;
    Ok(PgConnection::establish(&addr)?)
}
