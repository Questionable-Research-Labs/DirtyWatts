use serde::Deserialize;
use std::{collections::HashMap, env, fs::File, io::Write, path::PathBuf};

#[derive(Deserialize)]
struct PowerSite {
    connection_code: String,
    address: String,
    latitude: f64,
    longitude: f64,
}

fn main() {
    println!("cargo:rerun-if-changed=../power_sites.csv");
    // Generate static list of all available connection sites at compile time, bloats binary, but will speeds runtime
    println!("cargo:rerun-if-change=../power_sites.csv");
    let mut csv_file = csv::Reader::from_path("../power_sites.csv")
        .expect("Please make sure the power_sites.csv is in the root of the repo");

    let sites = csv_file
        .deserialize::<PowerSite>()
        .filter_map(|x| x.ok())
        .collect::<Vec<_>>();

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());

    let mut sites_map: HashMap<(i32, i32), Vec<PowerSite>> = HashMap::new();

    for site in sites {
        let lng = (site.longitude * 100.0).round() as i32;
        let lat = (site.latitude * 100.0).round() as i32;

        if sites_map.contains_key(&(lng, lat)) {
            let z = sites_map.get_mut(&(lng, lat)).unwrap();
            z.push(site);
        } else {
            sites_map.insert((lng, lat), vec![site]);
        }
    }

    let mut output_list = format!(
        "#[derive(serde::Serialize)]pub struct PointData {{ pub lng: f64,pub lat: f64,address: &'static str}}const ADDRESSES:[PointData; {}]=[",
        sites_map.len()
    );
    let mut output_map = "fn get_index(code: &str)->usize{match code{".to_string();

    for (index, ((lng, lat), sites)) in sites_map.iter().enumerate() {
        output_list += &format!(
            "PointData{{lng:{}f64, lat:{}f64, address:\"{}\"}},",
            (*lng as f64) / 100.0,
            (*lat as f64) / 100.0,
            sites[0].address
        );

        output_map += &format!(
            "{}=>{},",
            sites
                .iter()
                .map(|site| format!("\"{}\"", site.connection_code))
                .collect::<Vec<_>>()
                .join("|"),
            index
        );
    }

    output_list += "];";
    output_map += "_=>unreachable!()}}";

    let mut power_sites_file = File::create(out_path.join("power_sites.rs")).unwrap();

    write!(power_sites_file, "{output_list}{output_map}").unwrap();
}
