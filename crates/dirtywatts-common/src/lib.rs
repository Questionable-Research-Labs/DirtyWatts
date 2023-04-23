use chrono::{DateTime, FixedOffset};
use influxdb2::models::WriteDataPoint;
use serde::{Deserialize, Serialize};


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
    pub lng: f64,
    pub lat: f64,
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
            "emi_stat,code={},lat={},lng={} generation_mw={},load_mw={},mwh_price={} {}",
            self.connection_code.replace(" ", "\\ "),
            self.lat,
            self.lng,
            self.generation_mw,
            self.load_mw,
            self.mwh_price,
            self.time.timestamp_nanos()
        )
    }
}

#[derive(Debug, influxdb2::FromDataPoint, Default, Serialize)]
pub struct PowerReading {
    pub name: String,
    pub generation_mw: f64,
    pub capacity_mw: f64,
    pub time: DateTime<FixedOffset>,
}

impl WriteDataPoint for PowerReading {
    fn write_data_point_to<W>(&self, mut w: W) -> std::io::Result<()>
    where
        W: std::io::Write,
    {
        writeln!(
            w,
            "power_station,name={} generation_mw={},capacity_mw={} {}",
            self.name,
            self.generation_mw,
            self.capacity_mw,
            self.time.timestamp_nanos()
        )
    }
}

pub mod connection_points {
    include!(concat!(env!("OUT_DIR"), "/power_sites.rs"));
}
