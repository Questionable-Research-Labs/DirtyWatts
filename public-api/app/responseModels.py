from pydantic import BaseModel
from datetime import datetime
from typing import Union, List


class PowerStationStats(BaseModel):
    generation_mw: float
    capacity_mw: float


power_type_count = 8


class PowerTypes(BaseModel):
    battery: PowerStationStats
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