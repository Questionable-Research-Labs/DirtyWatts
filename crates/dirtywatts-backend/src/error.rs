use actix_web::{
    body::BoxBody,
    http::header::{self, HeaderValue},
    ResponseError,
};
use influxdb2::RequestError;
use serde::Serialize;
use utoipa::ToSchema;

#[derive(ToSchema, Serialize)]
pub struct InternalServerError {
    message: String,
}

#[derive(thiserror::Error, Debug)]
pub enum ApiError {
    #[error("Error accessing database: {0}")]
    DbError(#[from] RequestError),
}

impl ResponseError for ApiError {
    fn status_code(&self) -> actix_web::http::StatusCode {
        actix_web::http::StatusCode::INTERNAL_SERVER_ERROR
    }

    fn error_response(&self) -> actix_web::HttpResponse<BoxBody> {
        let mut res = actix_web::HttpResponse::new(self.status_code());

        res.headers_mut().append(
            header::CONTENT_TYPE,
            HeaderValue::from_static("application/json"),
        );

        let json_content = match self {
            ApiError::DbError(ex) => serde_json::to_string(&InternalServerError {
                message: format!("{ex}"),
            })
            .unwrap(),
        };

        let r#box = BoxBody::new(json_content);

        res.set_body(r#box)
    }
}
