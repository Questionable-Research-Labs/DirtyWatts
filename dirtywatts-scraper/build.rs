use std::{env, fs::File, io::Write, path::PathBuf};

use serde::Deserialize;

#[derive(Deserialize)]
struct PowerSite {
    pub connection_code: String,
}

fn main() {
    // Generate static list of all available connection sites at compile time, bloats binary, but will speeds runtime
    println!("cargo:rerun-if-change=../power_sites.csv");
    let mut csv_file = csv::Reader::from_path("../power_sites.csv")
        .expect("Please make sure the power_sites.csv is in the root of the repo");

    let sites = csv_file
        .deserialize::<PowerSite>()
        .filter_map(|x| x.ok())
        .collect::<Vec<_>>();

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());

    let output_file = format!(
        r#"const POWER_SITES: [&'static str; {}] = [{}];"#,
        sites.len(),
        sites
            .iter()
            .map(|x| format!("\"{}\"", x.connection_code))
            .collect::<Vec<_>>()
            .join(",")
    );

    let mut power_sites_file = File::create(out_path.join("power_sites.rs")).unwrap();
    power_sites_file.write_all(output_file.as_bytes()).unwrap();
}
