<script lang="ts">
    import { onMount } from "svelte";
    import "@carbon/styles/css/styles.css";
    import "@carbon/charts/styles.css";
    import { BarChartStacked, LineChart, StackedAreaChart } from "@carbon/charts-svelte";
    import type { PowerType } from "$lib/api";
    import { getPowerStations, getPowerStationsHistory } from "$lib/api";
    import { writable } from "svelte/store";
    import CoalPower from "$lib/CoalPower.svelte";

    interface Group {
        group: string;
        key: string;
        value: number
    }

    interface HistoryGroup {
        group: string;
        date: string;
        value: number;

    }

    const prettyNames: Record<string, string> = {
        "battery": "Battery",
        "co_gen": "CoGen",
        "coal": "Coal",
        "gas": "Gas",
        "geothermal": "Geothermal",
        "hydro": "Hydro",
        "diesel": "Diesel",
        "wind": "Wind"
    };

    const graphData = writable<Group[]>()
    const historyData = writable<HistoryGroup[]>()
    const coalPercent = writable<number>(0)


    async function updatePowerStations() {
        const {power_types} = await getPowerStations()
        let graphGroups: Group[] = [];
        let total = 0
        let coalValue = 0
        for (const key in power_types) {
            const {generation_mw, capacity_mw}: PowerType = power_types[key];
            const name = prettyNames[key] ?? key;

            total += generation_mw;
            if (key === "coal") {
                coalValue = generation_mw
            }

            graphGroups.push({
                group: "Generation (MW)",
                key: name,
                value: generation_mw
            })

            graphGroups.push({
                group: "Extra Capacity (MW)",
                key: name,
                value: capacity_mw-generation_mw
            })
        }

        $graphData = graphGroups
        $coalPercent = (coalValue / total) * 100
    }

    async function updateHistory() {
        let historyGroups: HistoryGroup[] = [];
        const response = await getPowerStationsHistory()
        for (let {timestamp, power_types} of response) {
            for (const key in power_types) {
                const {generation_mw}: PowerType = power_types[key];
                const name = prettyNames[key] ?? key;
                historyGroups.push({
                    date: timestamp,
                    value: generation_mw,
                    group: name
                })
            }
        }
        $historyData = historyGroups

    }


    onMount(async () => {

        await Promise.all([
            updatePowerStations(),
            updateHistory()
        ])

    })


</script>

<section class="section">

    {#if $coalPercent > 0}
        <CoalPower percent={$coalPercent}/>
    {/if}
    {#if $graphData != null}
        <h1 class="section__title">Generation & Capacity</h1>
        <div class="chart-wrapper">
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
        </div>
        <div class="chart-wrapper">

            <h1 class="section__title">NZ Power draw</h1>
            <StackedAreaChart
                    theme="g90"
                    data={$historyData}
                    options={{

                        height: "600px",
                        axes: {
                          left: {
                                "mapsTo": "value",
                                "title": "Generation (MW)",
                                "scaleType": "linear",
                                "stacked": true
                          },
                          bottom: {
                                "title": "",
                                "mapsTo": "date",
                                "scaleType": "time",
                              },
                     },
                        "curve": "curveMonotoneX",
                  }}
            />
        </div>
    {:else}
        <span class="loader"></span>
    {/if}

</section>

<style lang="scss">
  .chart-wrapper {
    max-width: 1000px;
    margin: 1rem auto;
    overflow: hidden;
    padding: 1rem;
  }
</style>