use bigdecimal::{BigDecimal, ToPrimitive};
use chrono::{DateTime, FixedOffset, Utc};
use dirtywatts_common::{ConnectionPoint, PowerMeasurement, PowerReadingPoint};
use futures::stream;
use influxdb2::Client;
use serde::Deserialize;
use sqlx::query_as;

const BUCKET_NAME: &'static str = "Dirty Watts";

#[derive(Deserialize)]
pub struct InfluxConfig {
    pub url: String,
    pub org: String,
    pub auth_token: String,
}

#[derive(Debug)]
struct PowerReadingsPostgres {
    name: String,
    generation: BigDecimal,
    capacity: BigDecimal,
    timestamp: DateTime<Utc>,
}

// impl Into<PowerReading> for self::PowerReadings {
//     fn into(self) -> PowerReading {
//         dirtywatts_common::PowerReading {
//             capacity_mw: self.capacity.to_f64().unwrap(),
//             generation_mw: self.generation.to_f64().unwrap(),
//             name: self.name,
//             time: self.timestamp.into(),
//         }
//     }
// }

#[derive(Debug)]
struct EmiStat {
    code: String,
    generation: BigDecimal,
    timestamp: DateTime<Utc>,
    mwh_price: BigDecimal,
    load: BigDecimal,
    lat: BigDecimal,
    lng: BigDecimal
}

impl Into<ConnectionPoint> for EmiStat {
    fn into(self) -> ConnectionPoint {
        ConnectionPoint {
            connection_code: self.code,
            time: self
                .timestamp
                .with_timezone(&FixedOffset::east_opt(0).unwrap()),
            load_mw: self.load.to_f64().unwrap(),
            generation_mw: self.generation.to_f64().unwrap(),
            mwh_price: self.mwh_price.to_f64().unwrap(),
            lat: self.lat.to_f64().unwrap(),
            lng: self.lng.to_f64().unwrap()
        }
    }
}

async fn app() {
    _ = dotenv::dotenv();
    let db_url = std::env::var("DATABASE_URL").unwrap();
    let connection = sqlx::PgPool::connect(db_url.as_str()).await.unwrap();

    println!("Fetching power readings");
    let power_readings = query_as!(PowerReadingsPostgres,
        r#"
        select r.generation as generation, r.capacity as capacity, r.reading_timestamp as "timestamp", s.kind as "name"
        from generation_levels as r
            join power_sources s on s.id = r.source_id
        "#).fetch_all(&connection).await.unwrap();

    println!("Done!\nFetching EMI stats");
    let emi_stats = query_as!(EmiStat, r#"
    select r.generation as generation, r.timestamp as "timestamp", r.load as "load", r.mwh_price as mwh_price, ns.connection_code as code, ns.latitude as lat, ns.longitude as lng
    from network_supply_reading as r
        join network_supply ns on r.connection_code = ns.connection_code;"#).fetch_all(&connection).await.unwrap();

    println!("Done!");

    let InfluxConfig {
        url,
        org,
        auth_token,
    } = envy::prefixed("INFLUXDB_")
        .from_env()
        .expect("Please set the variables INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_AUTH_TOKEN");

    let client = Client::new(url, org, auth_token);

    println!("Converting power readings");

    let chunked_readings: Vec<PowerReadingPoint> = power_readings.chunks(8).map(|reading_group| {
        let mut measurements = reading_group.iter().map(|x| (PowerMeasurement {
            capacity_mw: x.capacity.to_f64().unwrap(),
            generation_mw: x.generation.to_f64().unwrap(),
        }));

        return PowerReadingPoint{
            battery: measurements.next().unwrap(),
            co_gen: measurements.next().unwrap(),
            gas: measurements.next().unwrap(),
            coal: measurements.next().unwrap(),
            hydro: measurements.next().unwrap(),
            geothermal: measurements.next().unwrap(),
            diesel: measurements.next().unwrap(),
            wind: measurements.next().unwrap(),
            timestamp: reading_group[0].timestamp.into(),
        };
    }).collect();



    println!("Uploading power readings");
    client
        .write(
            BUCKET_NAME.into(),
            stream::iter(chunked_readings.into_iter()),
        )
        .await
        .unwrap();
    println!("Done!\nUploading emi stats");
    client
        .write(
            BUCKET_NAME.into(),
            stream::iter(emi_stats.into_iter().map(Into::<ConnectionPoint>::into)),
        )
        .await
        .unwrap();
    println!("Done!");
}

#[tokio::main]
async fn main() {
    app().await
}
