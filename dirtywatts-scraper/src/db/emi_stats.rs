use crate::db::models::{NetworkSupply, NetworkSupplyReading, NewNetworkSupplyReading};
use crate::emi_stats::ConnectionPoint;
use bigdecimal::{BigDecimal, FromPrimitive};
use chrono::{DateTime, Utc};
use diesel::prelude::*;

fn check_for_network_supply(cc: &str, connection: &mut PgConnection) -> bool {
    use super::schema::network_supply::dsl::*;
    network_supply
        .filter(connection_code.like(cc))
        .first::<NetworkSupply>(connection)
        .is_ok()
}

fn check_for_read_ts(ts: &DateTime<Utc>, connection: &mut PgConnection) -> bool {
    use super::schema::network_supply_reading::dsl::*;
    network_supply_reading
        .filter(timestamp.eq(ts))
        .first::<NetworkSupplyReading>(connection)
        .is_ok()
}

pub fn add_emi_stats(points: Vec<ConnectionPoint>, connection: &mut PgConnection) {
    use super::schema::network_supply_reading;

    println!("----------------------------------------");

    let group_timestamp = points[0].timestamp;

    if check_for_read_ts( &group_timestamp, connection) {
        println!("Data already uploaded for {}", group_timestamp);
        return;
    }

    for point in points {
        let trimed_connection_code = point.connection_code.split(" ").next().unwrap();


        if check_for_network_supply(trimed_connection_code, connection) {
                match diesel::insert_into(network_supply_reading::table)
                    .values(NewNetworkSupplyReading {
                        connection_code: trimed_connection_code.to_string(),
                        mwh_price: BigDecimal::from_f64(point.mwh_price).unwrap(),
                        timestamp: point.timestamp,
                        load: BigDecimal::from_f64(point.load_mw).unwrap(),
                        generation: BigDecimal::from_f64(point.generation_mw).unwrap(),
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
        } else {
            println!("Skipped {trimed_connection_code}")
        }
    }

    println!("----------------------------------------");
}
