use crate::{db::DbClient, utils::error_response};
use serde::Serialize;
use serde_json::json;
use worker::{Request, Response, RouteContext};

#[derive(Serialize)]
struct AliveResponse {
    alive: bool,
}

pub fn alive(_: Request, _: RouteContext<()>) -> Result<Response, worker::Error> {
    Response::from_json(&json!({"alive": true}))
}

pub async fn data_current(_: Request, ctx: RouteContext<()>) -> Result<Response, worker::Error> {
    let client = DbClient::new(&ctx.env);

    match client.has_data().await {
        Ok(alive) => Response::from_json(&json!({ "alive": alive })),
        Err(ex) => error_response(&ex, 500),
    }
}
