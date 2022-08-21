<script lang="ts">
    import { GaugeChart } from "@carbon/charts-svelte";
    import { derived } from "svelte/store";
    import type { PowerType } from "./api";
    import { powerTypes } from "./stores";

    let totalUsage = derived(powerTypes, ($powerTypes) => {
        if (!$powerTypes) {
            return 0;
        }
        let total = 0;
        for (const key in $powerTypes?.power_types) {
            const { generation_mw }: PowerType = $powerTypes?.power_types[key];
            total += generation_mw;
        }
        return total;
    });
    let totalCapacity = derived(powerTypes, ($powerTypes) => {
        if (!$powerTypes) {
            return 0;
        }
        let total = 0;
        for (const key in $powerTypes?.power_types) {
            const { capacity_mw }: PowerType = $powerTypes?.power_types[key];
            total += capacity_mw;
        }
        return total;
    });
    function replaceCenterNumber() {
        return `${($totalUsage/1000).toFixed(1)}/${($totalCapacity/1000).toFixed(1)} GW`;
    }
</script>

<div class="guage-container">
    <div class="guage-wrapper">
        <GaugeChart
            theme="g90"
            data={[
                {
                    group: "value",
                    value: ($totalUsage / $totalCapacity) * 100,
                },
            ]}
            options={{
                gauge: {
                    type: "semi",
                    status: "danger",
                    numberFormatter: replaceCenterNumber,
                    showPercentageSymbol: false,
                    arcWidth: 10,
                    valueFontSize: ()=>{return 45},
                },
                toolbar: {
                    enabled: false,
                },
            }}
        />
    </div>
</div>

<style lang="scss">
    .guage-container {
        display: flex;
        justify-content: center;
        align-items: stretch;
        flex-direction: row;
        margin-bottom: 1rem;
        height: 10em;
        .chart-wrapper {
            margin: 1rem auto;
            overflow: hidden;
            height: 100%;
            width: 100%;
        }
    }

</style>
