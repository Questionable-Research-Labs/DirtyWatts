<script lang="ts">
    import { LineChart } from "@carbon/charts-svelte";
    import { derived, writable, type Writable } from "svelte/store";
    import { onMount } from "svelte";
    import { getConnectionPointHistory, type ConnectionPoint } from "./api";
    export let connectionPointID: string;

    let dataset: Writable<ConnectionPoint[]> = writable([]);

    let formattedDataset = derived(dataset, ($dataset) => {
        return $dataset
            .map(({ timestamp, load_mw, generation_mw }) => {
                return [
                    {
                        group: "Load (MW)",
                        date: timestamp,
                        value: load_mw,
                    },
                    {
                        group: "Generation (MW)",
                        date: timestamp,
                        value: generation_mw,
                    },
                ];
            }).flat();
    });

    onMount(async () => {
        dataset.set(await getConnectionPointHistory(connectionPointID));
    });
</script>

<div class="chart-wrapper">
    <LineChart
        theme="g90"
        data={$formattedDataset}
        options={{
            height: "600px",
            axes: {
                left: {
                    mapsTo: "value",
                    title: "Power (MW)",
                    scaleType: "linear",
                },
                bottom: {
                    title: "",
                    mapsTo: "date",
                    scaleType: "time",
                },
            },
            curve: "curveMonotoneX",
        }}
    />
</div>
