// @generated automatically by Diesel CLI.

diesel::table! {
    emission_sources (id) {
        id -> Int4,
        gCO2e_per_kWh -> Float8,
    }
}

diesel::table! {
    generation_levels (id) {
        id -> Int4,
        source_id -> Int4,
        generation -> Numeric,
        capacity -> Numeric,
        reading_timestamp -> Timestamptz,
    }
}

diesel::table! {
    network_supply (connection_code) {
        #[max_length = 7]
        connection_code -> Varchar,
        address -> Text,
        network_region_id -> Nullable<Numeric>,
        network_region_name -> Nullable<Text>,
        #[max_length = 3]
        network_region_zone -> Nullable<Varchar>,
        latitude -> Numeric,
        longitude -> Numeric,
    }
}

diesel::table! {
    network_supply_reading (id) {
        id -> Int4,
        #[max_length = 7]
        connection_code -> Varchar,
        timestamp -> Timestamptz,
        load -> Numeric,
        generation -> Numeric,
        mwh_price -> Numeric,
    }
}

diesel::table! {
    power_sources (id) {
        id -> Int4,
        #[max_length = 15]
        kind -> Varchar,
    }
}

diesel::joinable!(emission_sources -> power_sources (id));
diesel::joinable!(generation_levels -> power_sources (source_id));
diesel::joinable!(network_supply_reading -> network_supply (connection_code));

diesel::allow_tables_to_appear_in_same_query!(
    emission_sources,
    generation_levels,
    network_supply,
    network_supply_reading,
    power_sources,
);
