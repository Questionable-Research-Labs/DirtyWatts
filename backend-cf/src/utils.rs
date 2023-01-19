use cfg_if::cfg_if;
use serde::Serialize;
use worker::{Headers, Response};

cfg_if! {
    // https://github.com/rustwasm/console_error_panic_hook#readme
    if #[cfg(feature = "console_error_panic_hook")] {
        extern crate console_error_panic_hook;
        pub use self::console_error_panic_hook::set_once as set_panic_hook;
    } else {
        #[inline]
        pub fn set_panic_hook() {}
    }
}

pub fn error_response<X>(x: &X, status: u16) -> worker::Result<Response>
where
    X: Serialize,
{
    Response::error(serde_json::to_string(x).unwrap(), status).map(|x| {
        x.with_headers({
            let mut headers = Headers::new();
            headers.append("Content-Type", "application/json").unwrap();
            headers
        })
    })
}
