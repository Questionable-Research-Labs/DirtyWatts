use actix_web::{
    get,
    web::{self, Data},
    Responder, Scope,
};
use influxdb2::Client;
use serde::Serialize;
use utoipa::ToSchema;

use crate::db::get_alive;

/// Is the API alive and well
#[derive(Serialize, ToSchema)]
pub struct ApiAlive {
    /// Is the API alive? If not, send mildly threatening emails to contact@jmw.nz ☺
    alive: bool,
}

/// What version of the API are we runnin
#[derive(Serialize, ToSchema)]
pub struct ApiStatus {
    /// The version as specified in Cargo.toml
    version: String,
    // build_date: String,
    /// The name of the package that has been built
    server: String,
}

impl Default for ApiStatus {
    fn default() -> Self {
        Self {
            version: env!("CARGO_PKG_VERSION").to_string(),
            server: env!("CARGO_PKG_NAME").to_string(),
        }
    }
}

/// Get some basic information about the currently running server
#[utoipa::path(get,
 path = "/status/info",
 tag = "Status",
 responses(
     (status = 200, description = "Current server version information", body = ApiStatus),
 ),
)]
#[get("/info")]
pub async fn info() -> impl Responder {
    web::Json(ApiStatus::default())
}

/// Is the server alive and well?
#[utoipa::path(get,
path = "/status/alive",
tag = "Status",
responses(
    (status = 200, description = "Yes the server is indeed functioning", body = ApiAlive),
),
)]
#[get("/alive")]
pub async fn alive() -> impl Responder {
    web::Json(ApiAlive { alive: true })
}

/// Is the server alive and well?
#[utoipa::path(get,
path = "/status/data_current",
tag = "Status",
responses(
    (status = 200, description = "Wether or not the data stored in the DB is up-to-date", body = ApiAlive),
),
)]
#[get("/data_current")]
pub async fn data_current(db: Data<Client>) -> impl Responder {
    let response = get_alive(db.get_ref()).await;
    println!("{:?}", response);
    web::Json(ApiAlive {
        alive: response.unwrap_or(false),
    })
}

pub fn setup_routes() -> Scope {
    Scope::new("/status")
        .service(info)
        .service(alive)
        .service(data_current)
}
