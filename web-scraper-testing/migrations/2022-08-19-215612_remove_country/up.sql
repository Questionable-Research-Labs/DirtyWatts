-- Your SQL goes here
ALTER TABLE power_sources DROP CONSTRAINT fk_country;
ALTER TABLE power_sources DROP COLUMN county_id;
DROP TABLE country;