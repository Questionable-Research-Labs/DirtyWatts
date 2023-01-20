mod status;

use dirtywatts_common::InfluxConfig;
use influxdb2::Client;
use rocket::{get, launch, routes, serde::json::Json};
use rocket_okapi::{
    openapi, openapi_get_routes,
    rapidoc::{make_rapidoc, GeneralConfig, HideShowConfig, RapiDocConfig},
    settings::UrlObject,
    swagger_ui::{make_swagger_ui, SwaggerUIConfig},
    JsonSchema,
};
use serde::Serialize;
use status::{alive, okapi_add_operation_for_alive_};

#[derive(Serialize, JsonSchema)]
/// Info about the current server
struct ApiInfo {
    /// Server name
    name: String,
    /// Server version
    version: String,
}

impl Default for ApiInfo {
    fn default() -> Self {
        Self {
            name: env!("CARGO_PKG_NAME").into(),
            version: env!("CARGO_PKG_VERSION").into(),
        }
    }
}

#[openapi(tag = "Status")]
#[get("/status/info")]
fn index() -> Json<ApiInfo> {
    Json(ApiInfo::default())
}

#[launch]
async fn launch() -> _ {
    _ = dotenv::dotenv();

    let InfluxConfig {
        url,
        org,
        auth_token,
    } = envy::prefixed("INFLUXDB_")
        .from_env()
        .expect("Please set the variables INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_AUTH_TOKEN");

    let client = Client::new(url, org, auth_token);

    rocket::build()
        // .mount("/", routes![index])
        .mount("/", openapi_get_routes![alive, index])
        .mount(
            "/",
            make_swagger_ui(&SwaggerUIConfig {
                url: "/openapi.json".to_string(),
                ..Default::default()
            }),
        )
        .mount(
            "/rapidoc/",
            make_rapidoc(&RapiDocConfig {
                general: GeneralConfig {
                    spec_urls: vec![UrlObject::new("General", "/openapi.json")],
                    ..Default::default()
                },
                hide_show: HideShowConfig {
                    allow_spec_url_load: false,
                    allow_spec_file_load: false,
                    ..Default::default()
                },
                ..Default::default()
            }),
        )
}
