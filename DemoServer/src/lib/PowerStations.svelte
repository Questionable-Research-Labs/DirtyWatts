<script context="module" lang="ts">
    // @carbon/charts/interfaces/enums.d.ts cannot be imported in modern ts
    enum ScaleTypes {
        TIME = "time",
        LINEAR = "linear",
        LOG = "log",
        LABELS = "labels",
        LABELS_RATIO = "labels-ratio",
    }
</script>

<script lang="ts">
    // Carbon Imports
	import '@carbon/styles/css/styles.css'
	import '@carbon/charts-svelte/styles.css'
    import { BarChartStacked, StackedAreaChart } from "@carbon/charts-svelte";
    import { Alignments, ChartTheme, LegendPositions } from "@carbon/charts";
    import type { AxesOptions, AxisOptions } from "@carbon/charts";

    import {cssRGBFormatter} from "$lib/utils";

    import { writable, type Writable } from "svelte/store";

    import { lightColour, powerTypes, powerTypesHistory, previewPosition } from "$lib/stores";
    import { AVERAGE_NZ_LOAD_MW } from "./consts";
    import type { PowerType } from './api';
    
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

    const historyData = writable<HistoryGroup[]>();


    powerTypesHistory.subscribe((powerTypesHistory) => {
        let historyGroups: HistoryGroup[] = [];
        for (let { timestamp, power_types } of powerTypesHistory) {
            for (const key in power_types) {
                const { generation_mw }: PowerType = power_types[key];
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
    let chartHeight = 800;


    let windowWidth = chartHeight;

    let mouseXPost = writable(0);
    let mousePreviewLineStart = 41;

    function updateMouseMove(event: MouseEvent) {
        mouseXPost.set(event.clientX);
    }
    
    function selectPosition(event: MouseEvent) {
        previewPosition.set(event.clientX / windowWidth);
    }

</script>

<svelte:window bind:innerWidth={windowWidth}/>



    <div class="chart-wrapper" on:mousemove={updateMouseMove} on:mousedown={selectPosition}>
        <svg id="chart-pos-canvas" style="--highlight-color: {cssRGBFormatter($lightColour)}">
            <line x1={$mouseXPost} y1={mousePreviewLineStart} x2={$mouseXPost} y2={chartHeight-mousePreviewLineStart/2} stroke="rgba(255,255,255,0.4)" stroke-width="2px"></line>
            <line id="previewLine" x1={$previewPosition*windowWidth} y1={mousePreviewLineStart} x2={$previewPosition*windowWidth} y2={chartHeight-mousePreviewLineStart/2} stroke="rgba(255,255,255,1)" stroke-width="2px"></line>
        </svg>
            <StackedAreaChart
                data={$historyData}
                options={{
                    height: `${chartHeight}px`,
                    axes: {
                        left: {
                            mapsTo: "value",
                            title: "Generation (MW)",
                            scaleType: ScaleTypes.LINEAR,
                            stacked: true,
                            thresholds: [
                                {
                                    value: AVERAGE_NZ_LOAD_MW,
                                    label: "Yearly Average",
                                    fillColor: "#ca5f5f",
                                },
                            ],
                            visible: false
                        },
                        bottom: {
                            title: "",
                            mapsTo: "date",
                            scaleType: ScaleTypes.TIME,

                        }
                    },
                    curve: "curveMonotoneX",
                    resizable: true,
                    toolbar: {
                        enabled: false
                    },
                    legend: {
                        enabled: true,
                        clickable: false,
                        alignment: Alignments.CENTER,
                        position: LegendPositions.TOP,

                    },
                    theme: ChartTheme.G90
                }}
            />
    </div>

<style lang="scss">
    .chart-wrapper {
        width: 100%;
        margin: 1rem auto;
        overflow: hidden;
        // padding: 1rem;
        text-align: left;
        color: white;
        background-color: #333;
        position: relative;

        #chart-pos-canvas {
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            z-index: 1;
            pointer-events: none;
            #previewLine {
                filter: drop-shadow(0 0 0.75rem var(--highlight-color)) drop-shadow(0 0 0.75rem var(--highlight-color));
            }
        }
    }
</style>
