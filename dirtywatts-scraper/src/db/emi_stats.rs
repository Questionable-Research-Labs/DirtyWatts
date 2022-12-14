use crate::db::models::{NetworkSupply, NetworkSupplyReading, NewNetworkSupplyReading};
use crate::emi_stats::ConnectionPoint;
use bigdecimal::BigDecimal;
use chrono::{DateTime, Utc};
use diesel::prelude::*;

fn check_for_network_supply(cc: &str, connection: &PgConnection) -> bool {
    use super::schema::network_supply::dsl::*;
    network_supply
        .filter(connection_code.like(cc))
        .first::<NetworkSupply>(connection)
        .is_ok()
}

fn check_for_read_ts(cc: &str, ts: &DateTime<Utc>, connection: &PgConnection) -> bool {
    use super::schema::network_supply_reading::dsl::*;
    network_supply_reading
        .filter(connection_code.like(cc))
        .filter(timestamp.eq(ts))
        .first::<NetworkSupplyReading>(connection)
        .is_ok()
}

pub fn add_emi_stats(points: Vec<ConnectionPoint>, connection: &PgConnection) {
    use super::schema::network_supply_reading;

    println!("----------------------------------------");

    for point in points {
        let trimed_connection_code = point.connection_code.split(" ").next().unwrap();

        if check_for_network_supply(trimed_connection_code, connection) {
            if !check_for_read_ts(trimed_connection_code, &point.timestamp, connection) {
                match diesel::insert_into(network_supply_reading::table)
                    .values(NewNetworkSupplyReading {
                        connection_code: trimed_connection_code.to_string(),
                        mwh_price: BigDecimal::from(point.mwh_price),
                        timestamp: point.timestamp,
                        load: BigDecimal::from(point.load_mw),
                        generation: BigDecimal::from(point.generation_mw),
                    })
                    .execute(connection)
                {
                    Ok(_) => {
                        println!("Uploaded data for {}", trimed_connection_code);
                    }
                    Err(ex) => {
                        println!("Error uploading data for {}: {ex}", trimed_connection_code);
                    }
                };
            }
        } else {
            println!("Skipped {trimed_connection_code}")
        }
    }

    println!("----------------------------------------");
}
