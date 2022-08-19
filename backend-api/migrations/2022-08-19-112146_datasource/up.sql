-- Your SQL goes here
CREATE TABLE country
(
    id           INT PRIMARY KEY,
    country_name TEXT NOT NULL
);

CREATE TABLE power_sources
(
    id        INT PRIMARY KEY,
    kind      VARCHAR(15) NOT NULL,
    county_id INT         NOT NULL,
    CONSTRAINT fk_country
        foreign key (county_id)
            references country (id)
);

CREATE TABLE generation_levels
(
    id                INT PRIMARY KEY,
    source_id         INT       NOT NULL,
    generation        DECIMAL   NOT NULL DEFAULT 0,
    capacity          DECIMAL   NOT NULL DEFAULT 0,
    reading_timestamp TIMESTAMP NOT NULL DEFAULT now(),
    CONSTRAINT fk_source
        FOREIGN KEY (source_id)
            REFERENCES power_sources (id)
);
