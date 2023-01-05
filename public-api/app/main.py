from glob import glob
import databases
import sqlalchemy
from sqlalchemy.orm import sessionmaker

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from starlette.responses import RedirectResponse

import db
import liveRoutes
import historyRoutes
import statusRoutes
from fastapi.openapi.utils import get_openapi

import os
from dotenv import load_dotenv
load_dotenv()

DATABASE_URL = os.environ.get("DATABASE_URL")
database = databases.Database(DATABASE_URL)


engine = sqlalchemy.create_engine(
    DATABASE_URL, pool_pre_ping=True
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

def custom_openapi():
    if app.openapi_schema:
        return app.openapi_schema
    openapi_schema = get_openapi(
        title="DirtyWatts Public API",
        version="0.0.1",
        description="This API is used to fetch both live and historical data from the DirtyWatts database. No authentication required.",
        routes=app.routes,
    )
    app.openapi_schema = openapi_schema
    return app.openapi_schema
app.openapi = custom_openapi


@app.on_event("startup")
async def startup():
    db.sessionMaker = sessionmaker(autocommit=False, autoflush=False, bind=engine)
    await database.connect()


@app.on_event("shutdown")
async def shutdown():
    await database.disconnect()

@app.get("/", include_in_schema=False)
async def redirect_to_docs():
    return RedirectResponse(url="/docs")

app.include_router(liveRoutes.router)
app.include_router(historyRoutes.router)
app.include_router(statusRoutes.router)