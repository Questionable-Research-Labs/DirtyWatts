use rocket::{get, serde::json::Json};
use rocket_okapi::{openapi, JsonSchema};
use serde::Serialize;

#[derive(Serialize, JsonSchema)]
pub struct Alive {
    /// Is the server alive? If false, huston, we have a problem, yell at contact@jmw.nz
    alive: bool,
}

#[openapi(tag = "Status")]
#[get("/status/alive")]
pub fn alive() -> Json<Alive> {
    Json(Alive { alive: true })
}
