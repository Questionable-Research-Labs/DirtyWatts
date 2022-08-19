table! {
    generation_levels (id) {
        id -> Int4,
        source_id -> Int4,
        generation -> Numeric,
        capacity -> Numeric,
        reading_timestamp -> Timestamp,
    }
}

table! {
    network_supply (id) {
        id -> Int4,
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
    power_sources (id) {
        id -> Int4,
        kind -> Varchar,
    }
}

joinable!(generation_levels -> power_sources (source_id));

allow_tables_to_appear_in_same_query!(
    generation_levels,
    network_supply,
    power_sources,
);
