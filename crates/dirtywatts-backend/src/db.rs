use dirtywatts_common::{PowerReading, BUCKET_NAME};
use influxdb2::{models::Query, Client, RequestError};
use log::debug;

pub async fn get_alive(client: &Client) -> Result<bool, RequestError> {
    let query = Query::new(format!(
        r#"from(bucket: "{BUCKET_NAME}")
    |> range(start: -2h)
    |> filter(fn: (r) => r._measurement == "power_station")
    |> last()"#
    ));

    // dbg!(&query);

    let result = client.query::<PowerReading>(Some(query)).await?;
    debug!("{:?}", result);
    Ok(!result.is_empty())
}

pub async fn get_live_powerstations(client: &Client) -> Result<Vec<PowerReading>, RequestError> {
    let query = Query::new(format!(
        r#"from(bucket: "{BUCKET_NAME}")
        |> range(start: -10h)
        |> filter(fn: (r) => r["_measurement"] == "power_station")"#
    ));

    dbg!(&query);

    let result = client.query::<PowerReading>(Some(query)).await?;
    debug!("Debug: {:?}", result);
    Ok(result)
}