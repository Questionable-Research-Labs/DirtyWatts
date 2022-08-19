-- This file should undo anything in `up.sql`
CREATE TABLE country
(
    id           INT PRIMARY KEY,
    country_name TEXT NOT NULL
);

ALTER TABLE power_sources
    ADD COLUMN county_id INT NOT NULL DEFAULT 1;
ALTER TABLE power_sources
    CONSTRAINT fk_country
    foreign key (county_id)
    references country (id);