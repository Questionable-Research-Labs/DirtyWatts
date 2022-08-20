<script lang="ts">
    import { onMount } from "svelte";
    import "@carbon/styles/css/styles.css";
    import "@carbon/charts/styles.css";
    import { BarChartStacked } from "@carbon/charts-svelte";
    import type { PowerType } from "$lib/api";
    import { getPowerStations } from "$lib/api";
    import { writable } from "svelte/store";
    import CoalPower from "$lib/CoalPower.svelte";

    interface Group {
        group: string;
        key: string;
        value: number
    }

    const graphData = writable<Group[]>()
    const coalPercent = writable<number>(0)

    onMount(async () => {
        const response = await getPowerStations()

        let data: Group[] = [];

        const powerTypes = response.power_types;
        const prettyNames = {
            "battery": "Battery",
            "co_gen": "CoGen",
            "coal": "Coal",
            "gas": "Gas",
            "geothermal": "Geothermal",
            "hydro": "Hydro",
            "liquid": "Liquid",
            "wind": "Wind"
        };

        for (const key in powerTypes) {
            const {generation_mw, capacity_mw}: PowerType = powerTypes[key];
            const name = prettyNames[key] ?? key;

            data.push({
                group: "Generation (MW)",
                key: name,
                value: generation_mw
            })

            data.push({
                group: "Capacity (MW)",
                key: name,
                value: capacity_mw
            })
        }

        let total = 0
        let coalValue = 0
        for (let key in powerTypes) {
            const {generation_mw}: PowerType = powerTypes[key];
            total += generation_mw;
            if (key === "coal") {
                coalValue = generation_mw
            }
        }

        console.log(total)

        $coalPercent = (coalValue / total) * 100

        $graphData = data

        $coalPercent = 20
    })


</script>

<div>
    <h1>Power Stations</h1>

    {#if $coalPercent > 1}
        <CoalPower percent={$coalPercent}/>
    {/if}
    {#if $graphData != null}
        <BarChartStacked
                theme="g90"
                data={$graphData}
                options={{

                        height: "600px",
                        axes: {
                          left: { scaleType: "labels", title: "Type" },
                          bottom: { stacked: true, title: "MW" },
                        },
                  }}
        />
    {:else}
        <h1>Loading Data</h1>
    {/if}

</div>

<style lang="scss">

</style>