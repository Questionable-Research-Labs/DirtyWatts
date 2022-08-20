use crate::db::models::{GenerationLevel, NewGenerationLevel, NewPowerSource, PowerSource};
use crate::power_station::{PowerStationType, PowerStationUpdatePackage, PowerTypes};
use anyhow::anyhow;
use bigdecimal::BigDecimal;
use chrono::{DateTime, Utc};
use diesel::prelude::*;

pub fn get_power_source(name: &str, connection: &PgConnection) -> anyhow::Result<PowerSource> {
    use super::schema::power_sources::{self, dsl::*};

    match power_sources
        .filter(kind.like(name))
        .first::<PowerSource>(connection)
    {
        Ok(vals) => Ok(vals),
        Err(diesel::result::Error::NotFound) => {
            println!("Entry not found, creating record");
            let val = diesel::insert_into(power_sources::table)
                .values(&NewPowerSource {
                    kind: name.to_string(),
                })
                .get_result::<PowerSource>(connection)?;
            Ok(val)
        }
        Err(ex) => Err(anyhow!(ex)),
    }
}

fn add_data(
    name: &str,
    value: PowerStationType,
    timestamp: &DateTime<Utc>,
    connection: &PgConnection,
) -> anyhow::Result<()> {
    use super::schema::generation_levels::{self, dsl::*};

    let PowerSource {
        id: power_source_id,
        ..
    } = get_power_source(name, connection)?;

    if let Err(diesel::NotFound) = generation_levels
        .filter(reading_timestamp.eq(timestamp))
        .filter(source_id.eq(power_source_id))
        .first::<GenerationLevel>(connection)
    {
        println!("Adding power sources data for {name}");
        diesel::insert_into(generation_levels::table)
            .values(NewGenerationLevel {
                source_id: power_source_id,
                generation: BigDecimal::from(value.generation_mw),
                capacity: BigDecimal::from(value.capacity_mw),
                reading_timestamp: timestamp.clone(),
            })
            .execute(connection)?;

        println!("Added power sources data {name}");
    } else {
        println!("Skipped adding data {name}");
    }

    Ok(())
}

pub fn add_readings(
    PowerStationUpdatePackage {
        power_types,
        timestamp,
    }: PowerStationUpdatePackage,
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
    } = power_types;

    add_data("battery", battery, &timestamp, connection)?;
    add_data("co_gen", co_gen, &timestamp, connection)?;
    add_data("gas", gas, &timestamp, connection)?;
    add_data("coal", coal, &timestamp, connection)?;
    add_data("hydro", hydro, &timestamp, connection)?;
    add_data("geothermal", geothermal, &timestamp, connection)?;
    add_data("liquid", liquid, &timestamp, connection)?;
    add_data("wind", wind, &timestamp, connection)?;

    Ok(())
}
