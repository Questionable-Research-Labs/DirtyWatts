from datetime import datetime, timedelta
from fastapi import APIRouter

from responseModels import CurrentData, PowerStationStats, PowerstationUpdatePackage, power_type_count, StatusTest
from typing import List
from sqlalchemy import func
import db

import asyncio

router = APIRouter(
    prefix="/status",
    tags=["Check Status"],
    responses={404: {"description": "Not found"}},
)


@router.get("/data_current", response_model=CurrentData)
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

    return {
        "healthy": healthy,
        "latest_update": updateTime.isoformat(),
    }



@router.get("/alive", response_model=StatusTest)
async def alive():
    return {"healthy": True}