from pydantic import BaseModel
from datetime import datetime
from typing import Union



class PowerStationStats(BaseModel):
    generation_mw: float
    capacity_mw: float

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
    network_region_id: Union[int, None]
    network_region_name: Union[str, None]
    network_region_zone: Union[str, None]
    address: str