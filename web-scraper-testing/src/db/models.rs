use super::schema::*;
use bigdecimal::BigDecimal;
use chrono::NaiveDateTime;

#[derive(Queryable)]
pub struct PowerSources {
    pub id: i32,
    pub kind: String,
}

#[derive(Insertable)]
#[table_name = "generation_levels"]
pub struct NewGenerationLevel {
    pub source_id: i32,
    pub generation: BigDecimal,
    pub capacity: BigDecimal,
    pub reading_timestamp: NaiveDateTime,
}

#[derive(Queryable)]
pub struct GenerationLevel {
    pub id: i32,
    pub source_id: i32,
    pub generation: BigDecimal,
    pub capacity: BigDecimal,
    pub reading_timestamp: NaiveDateTime,
}

#[derive(Insertable)]
#[table_name = "power_sources"]
pub struct NewPowerSource {
    pub kind: String,
}
