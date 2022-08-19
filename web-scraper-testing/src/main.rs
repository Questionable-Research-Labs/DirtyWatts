
use std::result::Result;

use crate::{power_station::get_current_power, emi_stats::get_emi_stats};

mod power_station;
mod emi_stats;
mod db;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>>{
    println!("POWERSTATION STATS:\n{:?}\n",get_current_power().await?);
    let connections = get_emi_stats().await?;
    println!("\nEMI STATS: (connections: {:})\n{:?}",connections.len(),connections[0]);
    Ok(())
}

