from datetime import datetime, timedelta
from fastapi import APIRouter
from refreshCache import refreshCache

from responseModels import PowerStationStats, PowerstationUpdatePackage, power_type_count, StatusTest
from typing import List
from sqlalchemy import func
import db

import asyncio

router = APIRouter(
    prefix="/status",
    tags=["Check Status"],
    responses={404: {"description": "Not found"}},
)


@router.get("/data_current", response_model=StatusTest)
async def power_stations():
    session = db.sessionMaker()
    latestRow = (
        session.query(db.generationLevels)
        .order_by(db.generationLevels.c.id.desc())
        .first()
    )
    updateTime = latestRow["reading_timestamp"]
    session.close()
    healthy = updateTime > datetime.now(updateTime.tzinfo) - timedelta(minutes=120)

    # Use as opportunity to test if there is a need to clear the cache
    asyncio.create_task(refreshCache(updateTime.isoformat()))

    return {
        "healthy": healthy,
        "latest_update": updateTime.isoformat(),
    }



@router.get("/alive", response_model=StatusTest)
async def alive():
    return {"healthy": True}