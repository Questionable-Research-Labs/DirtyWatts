from fastapi import APIRouter

from responseModels import PowerStationStats, PowerstationUpdatePackage, ConnectionPoint, power_type_count
from typing import List
from sqlalchemy import func
import db

router = APIRouter(
    prefix="/live",
    tags=["Live Data"],
    responses={404: {"description": "Not found"}},
)


@router.get("/power_stations", response_model=PowerstationUpdatePackage)
async def power_stations():
    session = db.sessionMaker()
    query = (
        session.query(db.generationLevels, db.powerSources)
        .join(db.powerSources, db.generationLevels.c.source_id == db.powerSources.c.id)
        .order_by(db.generationLevels.c.reading_timestamp.desc())
        .limit(power_type_count)
    )

    allPowerTypes = query.all()

    session.close()

    print(allPowerTypes[0]["kind"])
    updateTime = allPowerTypes[0]["reading_timestamp"]
    powerTypes = {}
    for i in allPowerTypes:
        powerTypes[i["kind"]] = PowerStationStats(
            generation_mw=i["generation"],
            capacity_mw=i["capacity"]
        )
    print(powerTypes)
    return PowerstationUpdatePackage(timestamp=updateTime, power_types=powerTypes)


@router.get("/grid_connection_points", response_model=List[ConnectionPoint])
async def power_stations():
    session = db.sessionMaker()

    subquery = session.query(db.networkSupplyReading).distinct(db.networkSupplyReading.c.connection_code).order_by(
        db.networkSupplyReading.c.connection_code, db.networkSupplyReading.c.timestamp.desc()).subquery()

    query = session.query(func.avg(subquery.c.mwh_price).label("mwh_price"),
                          func.sum(subquery.c.load).label("load"),
                          func.sum(subquery.c.generation).label("generation"),
                          func.string_agg(subquery.c.connection_code, ',').label("connection_code"),
                          func.string_agg(db.networkSupply.c.address, ', ').label("address"),
                          func.round(db.networkSupply.c.longitude, 1).label("longitude"),
                          func.round(db.networkSupply.c.latitude, 1).label("latitude"),
                          func.max(subquery.c.timestamp).label("timestamp")).join(
        db.networkSupply,
        db.networkSupply.c.connection_code == subquery.c.connection_code).group_by(
        func.round(db.networkSupply.c.longitude, 1), func.round(db.networkSupply.c.latitude, 1)).order_by(
        func.string_agg(subquery.c.connection_code, ','), func.max(subquery.c.timestamp).desc())

    from sqlalchemy.dialects import postgresql
    print(query.statement.compile(dialect=postgresql.dialect(), compile_kwargs={"literal_binds": True}))

    networkSupplyReadings = query.all()
    session.close()
    connectionPoints = []
    for i in networkSupplyReadings:
        # Remove any duplicate addresses
        unique = []
        for elm in i["address"].split(", "):
            if elm not in unique:
                unique.append(elm)

        connectionPoints.append(ConnectionPoint(
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

    return connectionPoints
