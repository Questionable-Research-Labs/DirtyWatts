from datetime import datetime
from xmlrpc.client import DateTime
from fastapi import APIRouter
from responseModels import PowerStationStats, ConnectionPoint, PowerstationUpdatePackage, power_type_count
from typing import List, Dict
import db
from utils import chunks

router = APIRouter(
    prefix="/history",
    tags=["Historical Data"],
    responses={404: {"description": "Not found"}},
)

@router.get("/power_stations", response_model=List[PowerstationUpdatePackage])
async def power_stations(start: datetime = datetime.min, end: datetime = datetime.max):
    query = (
        db.session.query(db.generationLevels, db.powerSources)
        .join(db.powerSources, db.generationLevels.c.source_id == db.powerSources.c.id)
        .order_by(db.generationLevels.c.reading_timestamp.desc())
        .filter(db.generationLevels.c.reading_timestamp <= end)
        .filter(db.generationLevels.c.reading_timestamp >= start)
    )
    allPowerTypes = list(chunks(query.all(),power_type_count))
    print(len(allPowerTypes))

    outputSlices = []

    for timeSlice in allPowerTypes:
        updateTime = timeSlice[0]["reading_timestamp"]
        powerTypes = {}
        for i in timeSlice:
            powerTypes[i["kind"]] = PowerStationStats(
                generation_mw=i["generation"],
                capacity_mw=i["capacity"]
            )
        outputSlices.append(PowerstationUpdatePackage(timestamp=updateTime, power_types=powerTypes))
    return outputSlices

@router.get("/grid_connection_points", response_model=Dict[str,List[ConnectionPoint]])
async def power_stations(start: datetime = datetime.min, end: datetime = datetime.max):
    query = (db.session
                .query(db.networkSupplyReading, db.networkSupply)
                .order_by(db.networkSupplyReading.c.connection_code, db.networkSupplyReading.c.timestamp.desc())
                .join(
                    db.networkSupply, db.networkSupply.c.connection_code == db.networkSupplyReading.c.connection_code)
    )
    networkSupplyReadings = query.all()
    allData = {

    }
    for i in networkSupplyReadings:
        point = (ConnectionPoint(
            connection_code=i["connection_code"],
            timestamp=i["timestamp"],
            load_mw=i["load"],
            generation_mw=i["generation"],
            mwh_price=i["mwh_price"],
            latitude=i["latitude"],
            longitude=i["longitude"],
            network_region_id=i["network_region_id"],
            network_region_name=i["network_region_name"],
            network_region_zone=i["network_region_zone"],
            address=i["address"]
        ))
        if point.connection_code not in allData:
            allData[point.connection_code] = []
        allData[point.connection_code].append(point)

    return allData
