from fastapi import APIRouter
from responseModels import PowerStationStats, PowerstationUpdatePackage, ConnectionPoint, power_type_count
from typing import List
import db
router = APIRouter(
    prefix="/live",
    tags=["Live Data"],
    responses={404: {"description": "Not found"}},
)

@router.get("/power_stations", response_model=PowerstationUpdatePackage)
async def power_stations():
    query = (
        db.session.query(db.generationLevels, db.powerSources)
        .join(db.powerSources, db.generationLevels.c.source_id == db.powerSources.c.id)
        .order_by(db.generationLevels.c.reading_timestamp.desc())
        .limit(power_type_count)
    )

    allPowerTypes = query.all()
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
    query = (db.session
                .query(db.networkSupplyReading, db.networkSupply)
                .order_by(db.networkSupplyReading.c.connection_code, db.networkSupplyReading.c.timestamp.desc())
                .distinct(db.networkSupplyReading.c.connection_code)
                .join(
                    db.networkSupply, db.networkSupply.c.connection_code == db.networkSupplyReading.c.connection_code)
    )
    networkSupplyReadings = query.all()
    connectionPoints = []
    for i in networkSupplyReadings:
        connectionPoints.append(ConnectionPoint(
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

    return connectionPoints
