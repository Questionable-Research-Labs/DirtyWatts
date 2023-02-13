import asyncio
from fastapi_cache import FastAPICache


lastUpdate = None

async def refreshCache(latestData: str):
    global lastUpdate
    if lastUpdate != latestData:
        lastUpdate = latestData
        print("New data, clearing cache")
        print(await FastAPICache.clear(namespace="generation-levels"))        