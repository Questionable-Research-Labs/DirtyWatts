-- This file should undo anything in `up.sql`
ALTER TABLE network_supply
    ALTER COLUMN network_region_id SET NOT NULL,
    ALTER COLUMN network_region_name SET NOT NULL,
    ALTER COLUMN network_region_zone SET NOT NULL;