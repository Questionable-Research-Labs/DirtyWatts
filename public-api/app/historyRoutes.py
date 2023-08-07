from datetime import datetime
from xmlrpc.client import DateTime
from fastapi import APIRouter
from responseModels import PowerStationStats, ConnectionPoint, PowerstationUpdatePackage, power_type_count
from typing import List, Dict
import db
from utils import chunks, round_sig
from sqlalchemy import func, desc, or_

from starlette.requests import Request
from starlette.responses import Response
from fastapi_cache.decorator import cache


router = APIRouter(
    prefix="/history",
    tags=["Historical Data"],
    responses={404: {"description": "Not found"}},
)

@router.get("/power_stations", response_model=List[PowerstationUpdatePackage])
@cache(namespace="generation-levels", expire=60*90)
async def power_stations(start: datetime = datetime.min, end: datetime = datetime.max,
                         time_interval_minutes: float = 30):
    """
    Gets the entire history of the power station type generation. It defaults to half-hourly data, which is the finest resolution available. 
    """
    update_interval = max(round(time_interval_minutes / 30), 1)

    session = db.sessionMaker()
    sub_query = session.query(
        db.generationLevels,
        func.row_number().over(order_by=desc(db.generationLevels.c.reading_timestamp)).label("row_number")
    ).subquery()

    query = (
        session.query(db.generationLevels, db.powerSources, db.emissionSources)
        .join(db.powerSources, db.generationLevels.c.source_id == db.powerSources.c.id)
        .join(db.emissionSources, db.powerSources.c.id == db.emissionSources.c.id)
        .order_by(db.generationLevels.c.reading_timestamp.desc())
        .filter(db.generationLevels.c.reading_timestamp <= end)
        .filter(db.generationLevels.c.reading_timestamp >= start)
        .join(sub_query, sub_query.c.id == db.generationLevels.c.id)
        .filter(func.div(sub_query.c.row_number - 1, power_type_count) % update_interval == 0)
    )
    session.close()
    allPowerTypes = list(chunks(query.all(), power_type_count))

    outputSlices = []

    MW_TO_KW = 1000
    TONNES_TO_GRAMS = 1000000

    for timeSlice in allPowerTypes:
        co2e_output = 0
        cumulative_power_output = 0
        co2e_intensity = 0
        updateTime = timeSlice[0]["reading_timestamp"]
        powerTypes = {}
        for i in timeSlice:
            co2e_output += (float(i["generation"]) * MW_TO_KW) * (float(i["gCO2e_per_kWh"]) / TONNES_TO_GRAMS)
            cumulative_power_output += float(i["generation"])  * MW_TO_KW
            powerTypes[i["kind"]] = PowerStationStats(
                generation_mw=i["generation"],
                capacity_mw=i["capacity"]
            )
        
        if cumulative_power_output > 0:
            co2e_intensity = (co2e_output*TONNES_TO_GRAMS) / cumulative_power_output

        outputSlices.append(PowerstationUpdatePackage(timestamp=updateTime, power_types=powerTypes, co2e_tonnne_per_hour=round_sig(co2e_output,5), co2e_grams_per_kwh=round_sig(co2e_intensity,5)))
    return outputSlices

@router.get("/grid_connection_points/{connection_code}", response_model=List[ConnectionPoint])
@cache(namespace="network-supply", expire=60*15)
async def power_stations(connection_code: str, start: datetime = datetime.min, end: datetime = datetime.max,
                         time_interval_minutes: float = 60):
    """
    Gets the entire history of a particular connection point, for example you can try "ABY0111". It defaults to hourly data, but you can go down to 15 minutely data. 
    """

    codes = connection_code.split(",")

    update_interval = max(round(time_interval_minutes / 15), 1)

    session = db.sessionMaker()

    connection_code_filter = db.networkSupplyReading.c.connection_code == codes[0]

    for elm in codes[1:]:
        connection_code_filter = connection_code_filter | (db.networkSupplyReading.c.connection_code == elm)

    all_query = (session
                 .query(db.networkSupplyReading, db.networkSupply)
                 .order_by(db.networkSupplyReading.c.connection_code, db.networkSupplyReading.c.timestamp.desc())
                 .filter(db.networkSupplyReading.c.timestamp <= end)
                 .filter(db.networkSupplyReading.c.timestamp >= start)
                 .filter(connection_code_filter)
                 .join(db.networkSupply,
                       db.networkSupply.c.connection_code == db.networkSupplyReading.c.connection_code)
                 ).subquery()

    networkSupplyReadings = session.query(all_query.c.timestamp,
                                          func.sum(all_query.c.load).label("load"),
                                          func.sum(all_query.c.generation).label("generation"),
                                          func.sum(all_query.c.mwh_price).label("mwh_price"),
                                          func.string_agg(all_query.c.connection_code, ',').label("connection_code"),
                                          func.string_agg(all_query.c.address, ', ').label("address"),
                                          func.round(all_query.c.latitude, 1).label("latitude"),
                                          func.round(all_query.c.longitude, 1).label("longitude")).group_by(
        all_query.c.timestamp, func.round(all_query.c.latitude, 1), func.round(all_query.c.longitude, 1)).order_by(all_query.c.timestamp.desc())

    session.close()

    networkSupplyReadingsFiltered = [x for i, x in enumerate(networkSupplyReadings) if i % update_interval == 0]

    allData = []
    for i in networkSupplyReadingsFiltered:
        # Remove any duplicate addresses
        unique = []
        for elm in i["address"].split(", "):
            if elm not in unique:
                unique.append(elm)

        allData.append(ConnectionPoint(
            connection_code=i["connection_code"],
            timestamp=i["timestamp"],
            load_mw=i["load"],
            generation_mw=i["generation"],
            mwh_price=i["mwh_price"],
            latitude=i["latitude"],
            longitude=i["longitude"],
            # network_region_id=[float(x) for x in i["network_region_id"].split(",")],
            # network_region_name=i["network_region_name"].split(","),
            # network_region_zone=i["network_region_zone"].split(","),
            address=', '.join(unique)
        ))

    return allData