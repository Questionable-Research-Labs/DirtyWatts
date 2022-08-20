from glob import glob
import databases
import sqlalchemy
from sqlalchemy.orm import Session

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from starlette.responses import RedirectResponse

import db
import liveRoutes

import os
from dotenv import load_dotenv
load_dotenv()

DATABASE_URL = os.environ.get("DATABASE_URL")
database = databases.Database(DATABASE_URL)



engine = sqlalchemy.create_engine(
    DATABASE_URL, pool_size=3, max_overflow=0
)
db.metadata.create_all(engine)

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
    db.session = Session(engine, future=True)
    await database.connect()


@app.on_event("shutdown")
async def shutdown():
    db.session.close()
    await database.disconnect()


@app.get("/")
async def redirect_to_docs():
    return RedirectResponse(url="/docs")

app.include_router(liveRoutes.router)
