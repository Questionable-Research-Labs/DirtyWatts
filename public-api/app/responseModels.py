from pydantic import BaseModel
from datetime import datetime
from typing import Union, List


class PowerStationStats(BaseModel):
    generation_mw: float
    capacity_mw: float


power_type_count = 7


class PowerTypes(BaseModel):
    co_gen: PowerStationStats
    coal: PowerStationStats
    gas: PowerStationStats
    geothermal: PowerStationStats
    hydro: PowerStationStats
    diesel: PowerStationStats
    wind: PowerStationStats


class PowerstationUpdatePackage(BaseModel):
    timestamp: datetime
    power_types: PowerTypes
    co2e_tonnne_per_hour: float
    co2e_grams_per_kwh: float


class ConnectionPoint(BaseModel):
    connection_code: str
    timestamp: datetime
    load_mw: float
    generation_mw: float
    mwh_price: float
    latitude: float
    longitude: float
    address: str

class StatusTest(BaseModel):
    healthy: bool
    
class CurrentData(StatusTest):
    latest_update: datetime