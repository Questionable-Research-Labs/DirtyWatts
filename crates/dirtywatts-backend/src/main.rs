mod db;
mod error;
mod routes;

use crate::routes::{live, status};

use actix_web::{middleware::Logger, web::Data, App, HttpServer};
use dirtywatts_common::InfluxConfig;
use dotenv::dotenv;
use env_logger::Env;
use influxdb2::Client;
use std::io;
use utoipa::OpenApi;
use utoipa_swagger_ui::SwaggerUi;


#[derive(OpenApi)]
#[openapi(
    paths(
        routes::status::info,
        routes::status::alive,
        routes::status::data_current,
        routes::live::power_stations
    ),
    components(schemas(
        routes::status::ApiStatus,
        routes::status::ApiAlive,
        error::InternalServerError,
        routes::live::PowerStationPoint,
        routes::live::PowerStation
    ))
)]
struct ApiDoc;

#[tokio::main]
async fn main() -> io::Result<()> {
    _ = dotenv();
    env_logger::init_from_env(Env::default().default_filter_or("info"));

    HttpServer::new(|| {
        App::new()
            .wrap(Logger::new("%r %Ts %b"))
            .app_data({
                let InfluxConfig {
                    url,
                    org,
                    auth_token,
                } = envy::prefixed("INFLUXDB_")
                    .from_env()
                    .expect("Please set the environment variables for InfluxDB");

                Data::new(Client::new(url, org, auth_token))
            })
            .service(status::setup_routes())
            .service(live::setup_routes())
            .service(SwaggerUi::new("/{_:.*}").url("/openapi.json", ApiDoc::openapi()))
    })
    .bind("0.0.0.0:8080")?
    .run()
    .await
}
