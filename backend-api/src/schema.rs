table! {
    country (id) {
        id -> Int4,
        country_name -> Text,
    }
}

table! {
    generationlevels (id) {
        id -> Int4,
        source_id -> Int4,
        generation -> Numeric,
        capacity -> Numeric,
        reading_timestamp -> Timestamp,
    }
}

table! {
    powersources (id) {
        id -> Int4,
        kind -> Varchar,
        county_id -> Int4,
    }
}

joinable!(generationlevels -> powersources (source_id));
joinable!(powersources -> country (county_id));

allow_tables_to_appear_in_same_query!(
    country,
    generationlevels,
    powersources,
);
