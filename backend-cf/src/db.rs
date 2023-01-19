use std::{error::Error, fmt::Display, iter::FromIterator, result};

use reqwest::{
    header::{self, HeaderMap, HeaderName, HeaderValue},
    Client, Request, Response,
};
use serde::{Deserialize, Serialize};
use worker::{console_error, console_log, Env};

#[derive(Debug, Deserialize, Serialize)]
pub struct InfluxError {
    code: String,
    message: String,
    err: Option<String>,
    op: Option<String>,
}

impl InfluxError {
    pub async fn from_response(response: Response) -> Self {
        response
            .json()
            .await
            .map_err(|ex| {
                console_error!("{ex}");
                InfluxError {
                    code: "invalid response".into(),
                    message: "The InfluxDB api returned an invalid api error response".into(),
                    err: None,
                    op: None,
                }
            })
            .and_then(|x| Result::<(), _>::Err(x))
            .unwrap_err()
    }
}

impl Display for InfluxError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Influx db return error {}: {}", self.code, self.message)
    }
}

impl Error for InfluxError {}

#[derive(Debug)]
pub struct DbClient {
    url: String,
    org: String,
    token: String,
}

impl DbClient {
    pub fn new(env: &Env) -> Self {
        Self {
            url: format!("{}/api/v2", env.var("INFLUXDB_URL").unwrap().to_string()),
            org: env.var("INFLUXDB_ORG").unwrap().to_string(),
            token: env.var("INFLUXDB_AUTH_TOKEN").unwrap().to_string(),
        }
    }

    fn get_client(&self) -> Client {
        Client::builder()
            .default_headers(HeaderMap::from_iter([
                (
                    header::AUTHORIZATION,
                    HeaderValue::from_str(format!("Token {}", self.token).as_str())
                        .expect("Couldn't set authorization header"),
                ),
                (header::ACCEPT, HeaderValue::from_static("application/json")),
                (
                    header::CONTENT_TYPE,
                    HeaderValue::from_static("application/vnd.flux"),
                ),
            ]))
            .build()
            .unwrap()
    }

    async fn query(&self, query_str: impl ToString) -> Result<Response, InfluxError> {
        let client = self.get_client();
        client
            .post(format!("{}/query?org={}", self.url, self.org))
            .body(query_str.to_string())
            .send()
            .await
            .map_err(|x| InfluxError {
                code: "fetch error".into(),
                message: format!("Error fetching: {x}"),
                err: None,
                op: None,
            })
    }

    pub async fn has_data(&self) -> Result<bool, InfluxError> {
        let results = self
            .query(
                r#"
            from(bucket: "Dirty Watts")
                |> range(start: -2h)
                |> filter(fn: (r) => r._measurement == "power_station")
            "#,
            )
            .await?;

        if results.status() != 200 {
            return Err(InfluxError::from_response(results).await);
        }

        let response = results.text().await.unwrap();

        Ok(response != "\r\n")
    }

    pub async fn live_power_sites(&self) -> Result<bool, InfluxError> {
        let results = self
            .query(
                r#"
        from(bucket: "Dirty Watts")
            |> filter(fn: (r) => r._measurement == "power_station")
            |> last()
        "#,
            )
            .await?;

        if results.status() != 200 {
            return Err(InfluxError::from_response(results).await);
        }

        todo!()
    }
}
