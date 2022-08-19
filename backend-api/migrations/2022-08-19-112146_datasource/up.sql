-- Your SQL goes here
CREATE TABLE Country
(
    id           INT PRIMARY KEY,
    country_name TEXT NOT NULL
);

CREATE TABLE PowerSources
(
    id        INT PRIMARY KEY,
    kind      VARCHAR(15) NOT NULL,
    county_id INT         NOT NULL,
    CONSTRAINT fk_country
        foreign key (county_id)
            references Country (id)
);

CREATE TABLE GenerationLevels
(
    id                INT PRIMARY KEY,
    source_id         INT       NOT NULL,
    generation        DECIMAL   NOT NULL DEFAULT 0,
    capacity          DECIMAL   NOT NULL DEFAULT 0,
    reading_timestamp TIMESTAMP NOT NULL DEFAULT now(),
    CONSTRAINT fk_source
        FOREIGN KEY (source_id)
            REFERENCES PowerSources (id)
);
