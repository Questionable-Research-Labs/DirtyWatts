from datetime import datetime
from typing import List
import databases
import sqlalchemy
from sqlalchemy.orm import Session

from fastapi import FastAPI, status
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import os
import urllib
from dotenv import load_dotenv
load_dotenv()

DATABASE_URL = os.environ.get("DATABASE_URL")
database = databases.Database(DATABASE_URL)

metadata = sqlalchemy.MetaData()

networkSupply = sqlalchemy.Table(
    "network_supply",
    metadata,
    sqlalchemy.Column("id", sqlalchemy.Integer, primary_key=True),
    sqlalchemy.Column("connection_code", sqlalchemy.VARCHAR(length=7)),
    sqlalchemy.Column("address", sqlalchemy.TEXT),
    sqlalchemy.Column("network_region_id", sqlalchemy.NUMERIC),
    sqlalchemy.Column("network_region_name", sqlalchemy.TEXT),
    sqlalchemy.Column("network_region_zone", sqlalchemy.VARCHAR(length=3)),
    sqlalchemy.Column("latitude", sqlalchemy.NUMERIC),
    sqlalchemy.Column("longitude", sqlalchemy.NUMERIC),
)

powerSources = sqlalchemy.Table(
    "power_sources",
    metadata,
    sqlalchemy.Column("id", sqlalchemy.Integer, primary_key=True),
    sqlalchemy.Column("kind", sqlalchemy.VARCHAR(length=15)),
)

generationLevels = sqlalchemy.Table(
    "generation_levels",
    metadata,
    sqlalchemy.Column("id", sqlalchemy.Integer, primary_key=True),
    sqlalchemy.Column("source_id", sqlalchemy.Integer),
    sqlalchemy.Column("generation", sqlalchemy.NUMERIC, default=0),
    sqlalchemy.Column("capacity", sqlalchemy.NUMERIC, default=0),
    sqlalchemy.Column("reading_timestamp", sqlalchemy.TIMESTAMP, default=sqlalchemy.func.now()),
)

engine = sqlalchemy.create_engine(
    DATABASE_URL, pool_size=3, max_overflow=0
)
metadata.create_all(engine)

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
    liquid: PowerStationStats
    wind: PowerStationStats

class PowerstationUpdatePackage(BaseModel):
    timestamp: datetime
    power_types: PowerTypes


class ConnectionPoint(BaseModel):
    connection_code: str
    timestamp: str
    load_mw: float
    generation_mw: float
    mwh_price: float

app = FastAPI(title="REST API using FastAPI PostgreSQL Async EndPoints")
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"]
)

@app.on_event("startup")
async def startup():
    await database.connect()

@app.on_event("shutdown")
async def shutdown():
    await database.disconnect()

@app.get("/power_stations", response_model=PowerstationUpdatePackage)
async def power_stations():
    session = Session(engine, future=True)
    query = session.query(generationLevels, powerSources).join(powerSources, generationLevels.c.source_id == powerSources.c.id)
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