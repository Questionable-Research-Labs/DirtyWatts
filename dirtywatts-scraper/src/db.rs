use futures::stream;
use influxdb2::{Client, RequestError};
use serde::Deserialize;

use crate::{emi_stats::ConnectionPoint, power_station::PowerStationUpdatePackage, BUCKET_NAME};

include!(concat!(env!("OUT_DIR"), "/power_sites.rs"));

#[derive(Debug, Deserialize)]
pub struct InfluxConfig {
    pub url: String,
    pub org: String,
    pub auth_token: String,
}

pub async fn add_emi_stats(
    connection_points: Vec<ConnectionPoint>,
    client: &Client,
) -> Result<(), RequestError> {
    client
        .write(
            BUCKET_NAME.into(),
            stream::iter(
                connection_points
                    .into_iter()
                    .filter(|x| POWER_SITES.contains(&x.connection_code.as_str())),
            ),
        )
        .await
}

pub async fn add_readings(
    readings: PowerStationUpdatePackage,
    client: &Client,
) -> Result<(), RequestError> {
    client
        .write(BUCKET_NAME.into(), stream::iter(readings.into_array()))
        .await
}
