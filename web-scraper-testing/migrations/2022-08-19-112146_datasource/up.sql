CREATE TABLE power_sources
(
    id   SERIAL PRIMARY KEY,
    kind VARCHAR(15) NOT NULL
);

CREATE TABLE generation_levels
(
    id                SERIAL PRIMARY KEY,
    source_id         INT                      NOT NULL,
    generation        DECIMAL                  NOT NULL DEFAULT 0,
    capacity          DECIMAL                  NOT NULL DEFAULT 0,
    reading_timestamp TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
    CONSTRAINT fk_source
        FOREIGN KEY (source_id)
            REFERENCES power_sources (id)
);

CREATE TABLE network_supply
(
    connection_code     VARCHAR(7) PRIMARY KEY,
    address             TEXT       NOT NULL,
    network_region_id   DECIMAL    NULL,
    network_region_name TEXT       NULL,
    network_region_zone VARCHAR(3) NULL,
    latitude            DECIMAL    NOT NULL,
    longitude           DECIMAL    NOT NULL
);

CREATE TABLE network_supply_reading
(
    id              SERIAL PRIMARY KEY,
    connection_code VARCHAR(7)               NOT NULL,
    timestamp       TIMESTAMP WITH TIME ZONE NOT NULL,
    load            DECIMAL                  NOT NULL,
    generation      DECIMAL                  NOT NULL,
    mwh_price       DECIMAL                  NOT NULL,
    CONSTRAINT connection_code_fk
        FOREIGN KEY (connection_code)
            REFERENCES network_supply (connection_code)
);

