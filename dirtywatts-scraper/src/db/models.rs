use super::schema::*;
use bigdecimal::BigDecimal;
use chrono::{DateTime, Utc};
use diesel::prelude::*;


#[derive(Insertable, Debug)]
#[table_name = "power_sources"]
pub struct NewPowerSource {
    pub kind: String,
}

#[derive(Queryable, Debug)]
pub struct PowerSource {
    pub id: i32,
    pub kind: String,
}

#[derive(Insertable, Debug)]
#[table_name = "generation_levels"]
pub struct NewGenerationLevel {
    pub source_id: i32,
    pub generation: BigDecimal,
    pub capacity: BigDecimal,
    pub reading_timestamp: DateTime<Utc>,
}

#[derive(Queryable, Debug)]
pub struct GenerationLevel {
    pub id: i32,
    pub source_id: i32,
    pub generation: BigDecimal,
    pub capacity: BigDecimal,
    pub reading_timestamp: DateTime<Utc>,
}

#[derive(Queryable)]
pub struct NetworkSupply {
    pub connection_code: String,
    pub address: String,
    pub network_region_id: Option<BigDecimal>,
    pub network_region_name: Option<String>,
    pub network_region_zone: Option<String>,
    pub latitude: BigDecimal,
    pub longitude: BigDecimal,
}

#[derive(Insertable)]
#[table_name = "network_supply_reading"]
pub struct NewNetworkSupplyReading {
    pub connection_code: String,
    pub timestamp: DateTime<Utc>,
    pub load: BigDecimal,
    pub generation: BigDecimal,
    pub mwh_price: BigDecimal,
}

#[derive(Queryable)]
pub struct NetworkSupplyReading {
    pub id: i32,
    pub connection_code: String,
    pub timestamp: DateTime<Utc>,
    pub load: BigDecimal,
    pub generation: BigDecimal,
    pub mwh_price: BigDecimal,
}
