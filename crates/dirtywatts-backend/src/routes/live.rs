use std::collections::HashMap;

use actix_web::{
    get,
    web::{self, Data},
    Responder, Scope,
};
use dirtywatts_common::PowerReading;
use influxdb2::Client;
use serde::Serialize;
use utoipa::ToSchema;


use crate::db::get_live_powerstations;

pub fn setup_routes() -> Scope {
    Scope::new("/live")
        .service(power_stations)
}
  

#[derive(Serialize, ToSchema)]
pub struct PowerStationPoint {
    pub timestamp: String,
    pub power_types: HashMap<String, PowerStation>,
}

#[derive(Serialize, ToSchema)]
pub struct PowerStation {
    pub generation_mw: f64,
    pub capacity_mw: f64,
}

#[utoipa::path(get,
    path = "/live/power_stations",
    tag = "Status",
    responses(
        (status = 200, description = "Live power data", body = PowerStationPoint),
    ),
    )]
#[get("/power_stations")]
pub async fn power_stations(db: Data<Client>) -> impl Responder {
    println!("Wjat");
    let response = get_live_powerstations(db.get_ref()).await;
    println!("yes {}",response.is_ok());

    println!("{:?}", response);
    web::Json(group_results(response.unwrap()))
}


fn group_results(readings: Vec<PowerReading>) -> PowerStationPoint {
    let mut grouped = HashMap::new();
    
    let mut timestamp = None;

    for reading in readings {
        if timestamp.is_none() {
            timestamp = Some(reading.time.to_string());
        }

        let power_type = reading.name;
        if grouped.contains_key(&power_type) {
            continue;
        }

        grouped.insert(power_type,PowerStation {
            generation_mw: reading.generation_mw,
            capacity_mw: reading.capacity_mw,
        });
    }
    PowerStationPoint {
        timestamp: timestamp.unwrap(),
        power_types: grouped,
    }
}