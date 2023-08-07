import sqlalchemy

sessionMaker = None

metadata = sqlalchemy.MetaData()

networkSupply = sqlalchemy.Table(
    "network_supply",
    metadata,
    sqlalchemy.Column("connection_code", sqlalchemy.VARCHAR(length=7)),
    sqlalchemy.Column("address", sqlalchemy.TEXT),
    sqlalchemy.Column("network_region_id", sqlalchemy.NUMERIC),
    sqlalchemy.Column("network_region_name", sqlalchemy.TEXT),
    sqlalchemy.Column("network_region_zone", sqlalchemy.VARCHAR(length=3)),
    sqlalchemy.Column("latitude", sqlalchemy.NUMERIC),
    sqlalchemy.Column("longitude", sqlalchemy.NUMERIC),
)

networkSupplyReading = sqlalchemy.Table(
    "network_supply_reading",
    metadata,
    sqlalchemy.Column("id", sqlalchemy.Integer, primary_key=True),
    sqlalchemy.Column("connection_code", sqlalchemy.VARCHAR(length=7)),
    sqlalchemy.Column("timestamp", sqlalchemy.TIMESTAMP),
    sqlalchemy.Column("load", sqlalchemy.NUMERIC),
    sqlalchemy.Column("generation", sqlalchemy.NUMERIC),
    sqlalchemy.Column("mwh_price", sqlalchemy.NUMERIC),
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
    sqlalchemy.Column("reading_timestamp", sqlalchemy.TIMESTAMP,
                      default=sqlalchemy.func.now()),
)

emissionSources = sqlalchemy.Table(
    "emission_sources",
    metadata,
    sqlalchemy.Column("id", sqlalchemy.Integer, sqlalchemy.ForeignKey("power_sources.id"), primary_key=True),
    sqlalchemy.Column("gCO2e_per_kWh", sqlalchemy.NUMERIC, default=0),
)