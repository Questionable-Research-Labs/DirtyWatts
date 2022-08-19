use crate::scraper::PowerStationUpdatePackage;
use diesel::{Connection, ConnectionResult, PgConnection};
use std::env;

mod schema;

pub fn create_connection() -> anyhow::Result<PgConnection> {
    let addr = env::var("DATABASE_URL")?;
    Ok(PgConnection::establish(&addr)?)
}

fn add_readings(
    update: PowerStationUpdatePackage,
    connection: &PgConnection,
) -> anyhow::Result<()> {
    for source in update. {

    }

    Ok(())
}
