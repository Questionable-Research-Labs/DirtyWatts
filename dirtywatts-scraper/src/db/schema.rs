table! {
    generation_levels (id) {
        id -> Int4,
        source_id -> Int4,
        generation -> Numeric,
        capacity -> Numeric,
        reading_timestamp -> Timestamptz,
    }
}

table! {
    network_supply (connection_code) {
        connection_code -> Varchar,
        address -> Text,
        network_region_id -> Nullable<Numeric>,
        network_region_name -> Nullable<Text>,
        network_region_zone -> Nullable<Varchar>,
        latitude -> Numeric,
        longitude -> Numeric,
    }
}

table! {
    network_supply_reading (id) {
        id -> Int4,
        connection_code -> Varchar,
        timestamp -> Timestamptz,
        load -> Numeric,
        generation -> Numeric,
        mwh_price -> Numeric,
    }
}

table! {
    power_sources (id) {
        id -> Int4,
        kind -> Varchar,
    }
}

joinable!(generation_levels -> power_sources (source_id));
joinable!(network_supply_reading -> network_supply (connection_code));

allow_tables_to_appear_in_same_query!(
    generation_levels,
    network_supply,
    network_supply_reading,
    power_sources,
);
