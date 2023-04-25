use actix_web::{
    get,
    web::{self, Data},
    Responder, Scope,
};
use influxdb2::Client;

use crate::{db::get_history_powerstations, routes::util::group_results};

pub fn setup_routes() -> Scope {
    Scope::new("/history")
        .service(power_stations)
        .service(grid_connection_points)
}

use chrono::{DateTime, FixedOffset};




#[utoipa::path(get,
    path = "/history/power_stations",
    tag = "History",
    responses(
        (status = 200, description = "History power data", body = PowerStationPoint),
    ),
    )]
#[get("/power_stations")]
pub async fn power_stations(db: Data<Client>) -> impl Responder {
    let response = get_history_powerstations(db.get_ref(), DateTime::default(),DateTime::default()).await;

    web::Json(group_results(response.unwrap()).unwrap())
}



#[utoipa::path(get,
    path = "/history/grid_connection_points",
    tag = "History",
    responses(
        (status = 200, description = "Todo", body = PowerStationPoint),
    ),
    )]
#[get("/grid_connection_points")]
pub async fn grid_connection_points(db: Data<Client>) -> impl Responder {
    format!("Hello, {}!", "world")
}
