from fastapi import APIRouter
from responseModels import PowerStationStats, PowerTypes, PowerstationUpdatePackage, power_type_count
from typing import List
import db
from utils import chunks
router = APIRouter(
    prefix="/history",
    tags=["history"],
    responses={404: {"description": "Not found"}},
)

@router.get("/power_stations")
async def power_stations():
    query = (
        db.session.query(db.generationLevels, db.powerSources)
        .join(db.powerSources, db.generationLevels.c.source_id == db.powerSources.c.id)
        .order_by(db.generationLevels.c.reading_timestamp.desc())
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
