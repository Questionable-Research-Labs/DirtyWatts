-- Your SQL goes here
create table emission_sources
(
    id              integer not null
        constraint emission_sources_pk
            primary key
        constraint emission_sources_power_sources_id_fk
            references power_sources,
    "gCO2e_per_kWh" float   not null
);