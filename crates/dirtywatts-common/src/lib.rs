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

// #[derive(Debug, influxdb2::FromDataPoint, Default, Serialize)]
// pub struct PowerReading {
//     pub name: String,
//     pub generation_mw: f64,
//     pub capacity_mw: f64,
//     pub time: DateTime<FixedOffset>,
// }

#[derive(Debug, influxdb2::FromDataPoint, Default, Serialize, Clone)]
pub struct PowerMeasurement {
    pub generation_mw: f64,
    pub capacity_mw: f64,
}

#[derive(Debug, Default, Serialize, Clone)]
pub struct PowerReadingPoint {
    pub battery: PowerMeasurement,
    pub co_gen: PowerMeasurement,
    pub coal: PowerMeasurement,
    pub gas: PowerMeasurement,
    pub geothermal: PowerMeasurement,
    pub hydro: PowerMeasurement,
    pub diesel: PowerMeasurement,
    pub wind: PowerMeasurement,
    pub timestamp: DateTime<FixedOffset>,
}

impl WriteDataPoint for PowerReadingPoint {
    fn write_data_point_to<W>(&self, mut w: W) -> std::io::Result<()>
    where
        W: std::io::Write,
    {
        let mut measurement_points = vec![];
        let format_value = |name: &str, point: &PowerMeasurement| -> String {
            format!(
                "{name}_generation_mw={},{name}_capacity_mw={}",
                point.generation_mw,
                point.capacity_mw,
            )
        };

        measurement_points.push(format_value("battery", &self.battery));
        measurement_points.push(format_value("co_gen", &self.co_gen));
        measurement_points.push(format_value("coal", &self.coal));
        measurement_points.push(format_value("gas", &self.gas));
        measurement_points.push(format_value("geothermal", &self.geothermal));
        measurement_points.push(format_value("hydro", &self.hydro));
        measurement_points.push(format_value("diesel", &self.diesel));
        measurement_points.push(format_value("wind", &self.wind));

        let record = format!("power_station,source=postgres {} {}",measurement_points.join(","), self.timestamp.timestamp_nanos());

        writeln!(w,"{}",record)
    }
}

pub mod connection_points {
    include!(concat!(env!("OUT_DIR"), "/power_sites.rs"));
}
