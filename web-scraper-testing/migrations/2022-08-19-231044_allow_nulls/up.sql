-- Your SQL goes here
ALTER TABLE network_supply
    ALTER COLUMN network_region_id DROP NOT NULL,
    ALTER COLUMN network_region_name DROP NOT NULL,
    ALTER COLUMN network_region_zone DROP NOT NULL;