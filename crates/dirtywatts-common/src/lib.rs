use chrono::{DateTime, FixedOffset, Utc};
use influxdb2::models::WriteDataPoint;
use serde::Deserialize;

pub const BUCKET_NAME: &'static str = "Dirty Watts";

#[derive(Debug, Deserialize)]
pub struct InfluxConfig {
    pub url: String,
    pub org: String,
    pub auth_token: String,
}

#[derive(Clone, Debug, influxdb2::FromDataPoint, Default)]
pub struct ConnectionPoint {
    pub connection_code: String,
    pub time: DateTime<FixedOffset>,
    pub load_mw: f64,
    pub generation_mw: f64,
    pub mwh_price: f64,
}

impl WriteDataPoint for ConnectionPoint {
    fn write_data_point_to<W>(&self, mut w: W) -> std::io::Result<()>
    where
        W: std::io::Write,
    {
        writeln!(
            w,
            "emi_stat,code={} generation_mw={},load_mw={},mwh_price={} {}",
            self.connection_code.replace(" ", "\\ "),
            self.generation_mw,
            self.load_mw,
            self.mwh_price,
            self.time.timestamp_nanos()
        )
    }
}

#[derive(Debug)]
pub struct PowerReading {
    pub name: String,
    pub generation: f64,
    pub capacity: f64,
    pub timestamp: DateTime<Utc>,
}

impl WriteDataPoint for PowerReading {
    fn write_data_point_to<W>(&self, mut w: W) -> std::io::Result<()>
    where
        W: std::io::Write,
    {
        writeln!(
            w,
            "power_station,type={} generation_mw={},capacity_mw={} {}",
            self.name,
            self.generation,
            self.capacity,
            self.timestamp.timestamp_nanos()
        )
    }
}

pub mod connection_points {
    include!(concat!(env!("OUT_DIR"), "/power_sites.rs"));
}
