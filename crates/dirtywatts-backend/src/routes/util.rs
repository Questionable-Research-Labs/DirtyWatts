use serde::Serialize;
use utoipa::ToSchema;
use dirtywatts_common::PowerReading;
use std::collections::HashMap;


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

pub fn group_results(readings: Vec<PowerReading>) -> Option<PowerStationPoint> {
    let mut grouped = HashMap::new();
    
    if readings.is_empty() {
        return None;
    }
    let timestamp = readings[0].time.to_string();

    for reading in readings {

        let power_type = reading.name;
        if grouped.contains_key(&power_type) {
            continue;
        }

        grouped.insert(power_type,PowerStation {
            generation_mw: reading.generation_mw,
            capacity_mw: reading.capacity_mw,
        });
    }
    Some(PowerStationPoint {
        timestamp: timestamp,
        power_types: grouped,
    })
}