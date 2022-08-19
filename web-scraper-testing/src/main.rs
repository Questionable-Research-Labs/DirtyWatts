
use std::result::Result;

use crate::{power_station::get_current_power, emi_stats::get_emi_stats};

mod power_station;
mod emi_stats;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>>{
    println!("POWERSTATION STATS:\n{:?}",get_current_power().await?);
    println!("EMI STATS:\n{:?}",get_emi_stats().await?);
    Ok(())
}

