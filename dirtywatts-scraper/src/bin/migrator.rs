use chrono::{DateTime, Utc};
use futures::stream;
use influxdb2::{models::WriteDataPoint, Client};
use serde::Deserialize;
use sqlx::query_as;
use bigdecimal::BigDecimal;

const BUCKET_NAME: &'static str = "Dirty Watts";

#[derive(Deserialize)]
pub struct InfluxConfig {
    pub url: String,
    pub org: String,
    pub auth_token: String,
}

#[derive(Debug)]
struct PowerReadings {
    name: String,
    generation: BigDecimal,
    capacity: BigDecimal,
    timestamp: DateTime<Utc>,
}

impl WriteDataPoint for PowerReadings {
    fn write_data_point_to<W>(&self, mut w: W) -> std::io::Result<()>
    where
        W: std::io::Write {
        writeln!(
            w,
            "power_station,type={} generation_mw={},capacity_mw={} {}",
            self.name, self.generation, self.capacity, self.timestamp.timestamp_nanos()
        )
    }
}

#[derive(Debug)]
struct EmiStat {
    code: String,
    generation: BigDecimal,
    timestamp: DateTime<Utc>,
    mwh_price: BigDecimal,
    load: BigDecimal 
}
impl WriteDataPoint for EmiStat {
    fn write_data_point_to<W>(&self, mut w: W) -> std::io::Result<()>
    where
        W: std::io::Write,
    {
        writeln!(
            w,
            "emi_stat,code={} generation_mw={},load_mw={},mwh_price={} {}",
            self.code.replace(" ", "\\ "),
            self.generation,
            self.load,
            self.mwh_price,
            self.timestamp.timestamp_nanos()
        )
    }
}

async fn app() {
    _ = dotenv::dotenv();
    let db_url = std::env::var("DATABASE_URL").unwrap();
    let connection = sqlx::PgPool::connect(db_url.as_str()).await.unwrap();
    
    println!("Fetching power readings");
    let power_readings = query_as!(PowerReadings, 
        r#"
        select r.generation as generation, r.capacity as capacity, r.reading_timestamp as "timestamp", s.kind as "name"
        from generation_levels as r
            join power_sources s on s.id = r.source_id
        "#).fetch_all(&connection).await.unwrap();
    println!("Done!\nFetching EMI stats");
    let emi_stats = query_as!(EmiStat, r#"
    select r.generation as generation, r.timestamp as "timestamp", r.load as "load", r.mwh_price as mwh_price, ns.connection_code as code
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
    
    println!("Uploading power readings");
    client.write(BUCKET_NAME.into(), stream::iter(power_readings)).await.unwrap();
    println!("Done!\nUploading emi stats");
    client.write(BUCKET_NAME.into(), stream::iter(emi_stats)).await.unwrap();
    println!("Done!");
}

#[tokio::main]
async fn main() {
    app().await
}
