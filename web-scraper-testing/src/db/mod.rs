use crate::scraper::defs::PowerTypes;
use crate::scraper::{PowerStationType, PowerStationUpdatePackage};
use diesel::{Connection, ConnectionResult, PgConnection};
use std::env;

mod models;
mod schema;

pub fn create_connection() -> anyhow::Result<PgConnection> {
    let addr = env::var("DATABASE_URL")?;
    Ok(PgConnection::establish(&addr)?)
}

fn add_readings(
    update: PowerStationUpdatePackage,
    connection: &PgConnection,
) -> anyhow::Result<()> {
    let PowerTypes {
        battery,
        co_gen,
        gas,
        coal,
        hydro,
        geothermal,
        liquid,
        wind,
    } = update.power_types;

    if

    Ok(())
}
