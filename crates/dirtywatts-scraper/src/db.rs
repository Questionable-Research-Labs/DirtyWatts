use crate::power_station::PowerStationUpdatePackage;
use dirtywatts_common::{ConnectionPoint, BUCKET_NAME};
use futures::stream;
use influxdb2::{Client, RequestError};

pub async fn add_emi_stats(
    connection_points: Vec<ConnectionPoint>,
    client: &Client,
) -> Result<(), RequestError> {
    client
        .write(BUCKET_NAME.into(), stream::iter(connection_points))
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
