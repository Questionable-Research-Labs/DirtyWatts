from datetime import datetime
from xmlrpc.client import DateTime
from fastapi import APIRouter
from responseModels import PowerStationStats, ConnectionPoint, PowerstationUpdatePackage, power_type_count
from typing import List, Dict
import db
from utils import chunks
from sqlalchemy import func, desc
import math

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


# Example: ABY0111
@router.get("/grid_connection_points/{connection_code}", response_model=List[ConnectionPoint])
async def power_stations(connection_code: str, start: datetime = datetime.min, end: datetime = datetime.max, time_interval_minutes: float = 60):
    update_interval = max(round(time_interval_minutes/15),1)

    sub_query = db.session.query(
        db.networkSupplyReading,
        func.row_number().over(order_by=desc(db.networkSupplyReading.c.timestamp)).label("row_number")
        ).subquery()

    query = (db.session
                .query(db.networkSupplyReading, db.networkSupply)
                .order_by(db.networkSupplyReading.c.connection_code, db.networkSupplyReading.c.timestamp.desc())
                .filter(db.networkSupplyReading.c.timestamp <= end)
                .filter(db.networkSupplyReading.c.timestamp >= start)
                .filter(db.networkSupplyReading.c.connection_code == connection_code)
                .join(
                    db.networkSupply, db.networkSupply.c.connection_code == db.networkSupplyReading.c.connection_code)
                .join(sub_query, sub_query.c.id == db.networkSupplyReading.c.id)
                .filter(sub_query.c.row_number % update_interval == 0)
    )
    networkSupplyReadings = query.all()
    allData = []
    for i in networkSupplyReadings:
        allData.append(ConnectionPoint(
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

    return allData
