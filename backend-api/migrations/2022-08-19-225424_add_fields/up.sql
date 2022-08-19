-- Your SQL goes here
CREATE TABLE network_supply
(
    id                  SERIAL PRIMARY KEY,
    connection_code     VARCHAR(7) NOT NULL,
    address             TEXT       NOT NULL,
    network_region_id   DECIMAL    NOT NULL,
    network_region_name TEXT       NOT NULL,
    network_region_zone VARCHAR(3) NOT NULL,
    latitude            DECIMAL    NOT NULL,
    longitude           DECIMAL    NOT NULL
);