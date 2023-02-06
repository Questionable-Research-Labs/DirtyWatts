<script context="module" lang="ts">
    // @carbon/charts/interfaces/enums.d.ts cannot be imported in modern ts
    enum ScaleTypes {
		TIME = "time",
		LINEAR = "linear",
		LOG = "log",
		LABELS = "labels",
		LABELS_RATIO = "labels-ratio"
	}

</script>
<script lang="ts">
    // Carbon Imports
    import "@carbon/styles/css/styles.css";
    import "@carbon/charts/styles.css";
    import { BarChartStacked, StackedAreaChart} from "@carbon/charts-svelte";
    import type {AxesOptions, AxisOptions} from "@carbon/charts/interfaces";

    import { writable } from "svelte/store";

    import type { PowerType } from "$lib/api";
    import { powerTypes, powerTypesHistory } from "$lib/stores";


    interface Group {
        group: string;
        key: string;
        value: number;
    }

    interface HistoryGroup {
        group: string;
        date: string;
        value: number;
    }


    const prettyNames: Record<string, string> = {
        battery: "Battery",
        co_gen: "CoGen",
        coal: "Coal",
        gas: "Gas",
        geothermal: "Geothermal",
        hydro: "Hydro",
        diesel: "Diesel",
        wind: "Wind",
    };

    const cleanPower: string[] = ['hydro', 'wind', 'geothermal']
    const dirtyPower: string[] = ['batter', 'co_gen', 'coal', 'gas', 'diesel']

    const cleanGraphData = writable<Group[]>();
    const dirtyGraphData = writable<Group[]>();

    const historyData = writable<HistoryGroup[]>();

    powerTypes.subscribe((powerTypes) => {
        if (!powerTypes) {
            return;
        }
        let cleanGroups: Group[] = [];
        let dirtyGroups: Group[] = [];
        let total = 0;
        let coalValue = 0;
        for (const key in powerTypes?.power_types) {
            const {generation_mw, capacity_mw}: PowerType =
                powerTypes?.power_types[key];
            const name = prettyNames[key] ?? key;


            total += generation_mw;
            if (key === "coal") {
                coalValue = generation_mw;
            }

            if (cleanPower.indexOf(key) != -1) {
                cleanGroups.push({
                    group: "Generation (MW)",
                    key: name,
                    value: generation_mw,
                });

                cleanGroups.push({
                    group: "Extra Capacity (MW)",
                    key: name,
                    value: capacity_mw - generation_mw,
                });
            } else if (dirtyPower.indexOf(key) != -1) {
                dirtyGroups.push({
                    group: "Generation (MW)",
                    key: name,
                    value: generation_mw,
                });

                dirtyGroups.push({
                    group: "Extra Capacity (MW)",
                    key: name,
                    value: capacity_mw - generation_mw,
                });
            }

        }

        $cleanGraphData = cleanGroups
        $dirtyGraphData = dirtyGroups
    });

    powerTypesHistory.subscribe((powerTypesHistory) => {
        let historyGroups: HistoryGroup[] = [];
        for (let {timestamp, power_types} of powerTypesHistory) {
            for (const key in power_types) {
                const {generation_mw}: PowerType = power_types[key];
                const name = prettyNames[key] ?? key;
                historyGroups.push({
                    date: timestamp,
                    value: generation_mw,
                    group: name,
                });
            }
        }
        $historyData = historyGroups;
    });

    let windowWidth = 0;
    $: mobileMode = windowWidth < 450;
    let generationCapacityAxes: AxesOptions<AxisOptions>;
    $: generationCapacityAxes = mobileMode ? {
        bottom: { scaleType: ScaleTypes.LABELS, title: "Type" },
        left: { stacked: true, title: "MW" },
    } : {
        left: { scaleType: ScaleTypes.LABELS, title: "Type" },
        bottom: { stacked: true, title: "MW" },
    };
</script>

<svelte:window bind:innerWidth={windowWidth} />

<section class="section section--fit">
    <h1 class="section__title">Generation & Capacity</h1>
    <p class="section__text">The graphs below show the different power sources the purple bar indicates how much power
        is currently being
        generated in MW (Megawatts) and the cyan bar indicates the extra capacity </p>
    {#if $cleanGraphData != null}
        <div class="chart-wrapper">
            <h2 class="chart__title">Clean Power</h2>
            {#key mobileMode}
                <BarChartStacked
                        theme="g90"
                        data={$cleanGraphData}
                        options={{
                        height: "400px",
                        axes: generationCapacityAxes,
                        title: `Last Updated: ${new Date($powerTypes?.timestamp || "").toLocaleString()}`,
                    }}
                />
            {/key}
        </div>
        {#if $dirtyGraphData != null}
            <div class="chart-wrapper">
                <h2 class="chart__title">Dirty Power</h2>
                {#key mobileMode}
                    <BarChartStacked
                            theme="g90"
                            data={$dirtyGraphData}
                            options={{
                        height: "400px",
                        axes: generationCapacityAxes,
                        title: `Last Updated: ${new Date($powerTypes?.timestamp || "").toLocaleString()}`
                    }}
                    />
                {/key}
            </div>
        {/if}
    {:else}
        <span class="loader"></span>
    {/if}

    <div class="chart-wrapper">
        <h1 class="section__title">NZ Power draw</h1>
        <p class="section__text">The graph below shows historical data for what the power generation for each source
            was
            at different dates and times </p>
        {#if $historyData != null}
            <StackedAreaChart
                    theme="g90"
                    data={$historyData}
                    options={{
                height: "600px",
                axes: {
                    left: {
                        mapsTo: "value",
                        title: "Generation (MW)",
                        scaleType: ScaleTypes.LINEAR,
                        stacked: true,
                    },
                    bottom: {
                        title: "Date",
                        mapsTo: "date",
                        scaleType: ScaleTypes.TIME,
                    },
                },
                curve: "curveMonotoneX",
                title: `Last Updated: ${new Date($powerTypes?.timestamp || "").toLocaleString()}`
            }}
            />
        {:else}
            <span class="loader"></span>
        {/if}
    </div>
</section>

<style lang="scss">
  .chart-wrapper {
    max-width: 1000px;
    margin: 1rem auto;
    overflow: hidden;
    padding: 1rem;
    text-align: left;
  }

  .chart__title {
    font-size: 1.5rem;
    color: white;
    font-weight: bold;
    text-align: center;
  }
</style>
