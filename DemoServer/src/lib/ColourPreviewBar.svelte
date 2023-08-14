<script lang="ts">
    import { onDestroy, onMount } from "svelte";
    import type { PowerStationsResponse, PowerType } from "./api";
    import { calculateLightColour, calculateLightColourOld } from "./calculateLightColour";
    import { newLightColorSystem, powerTypesHistory } from "./stores";
    import { cssRGBFormatter, findClosest, type RGBColor } from "./utils";


    export let height = 800;
    export let width = 800;

    let canvasRef: HTMLCanvasElement;

    function renderCanvas(powerTypesHistory: PowerStationsResponse[]) {
        if (!canvasRef) return;
        const ctx = canvasRef.getContext("2d");
        if (!ctx) return;
        ctx.clearRect(0, 0, width, height);
        ctx.fillStyle = "white";
        ctx.fillRect(0, 0, width, height);

        const percent = 1 / powerTypesHistory.length;
        let x = 0;
        for (const datapoint of powerTypesHistory) {
            let colour = [0,0,0] as RGBColor;
            if ($newLightColorSystem) {
                colour = calculateLightColour(datapoint);
            } else {
                colour = calculateLightColourOld(datapoint.power_types);
            }
            ctx.fillStyle = cssRGBFormatter(colour);
            ctx.fillRect(x, 0, width * percent, height);
            ctx.strokeStyle = "#ffffff";
            ctx.lineWidth   = 1;
            ctx.strokeRect(x, 0, width * percent, height);
            x += width * percent;
        }

        // for (let i = 0; i < width; i++) {
        //     let percent = i / width;
        //     let datapoint = findClosest(powerTypesHistory, percent);
        //     if (!datapoint) continue;
        //     let colour = [0,0,0] as RGBColor;
        //     if ($newLightColorSystem) {
        //         colour = calculateLightColour(datapoint);
        //     } else {
        //         colour = calculateLightColourOld(datapoint.power_types);
        //     }
        //     ctx.strokeStyle = cssRGBFormatter(colour);
        //     ctx.lineWidth = 1;
        //     ctx.beginPath();
        //     ctx.moveTo(i, 0);
        //     ctx.lineTo(i, height);
        //     ctx.stroke();
        // }
    }

    let clean = powerTypesHistory.subscribe(renderCanvas);
    let clean2 = newLightColorSystem.subscribe(()=>{renderCanvas($powerTypesHistory)});
    let clean3: NodeJS.Timer;

    onMount(()=>{
        renderCanvas($powerTypesHistory);
        clean3 = setInterval(()=>{
            renderCanvas($powerTypesHistory);
        }, 2000);
    });

    onDestroy(()=>{
        clean();
        clean2();
        clearInterval(clean3);
    })
</script>

<canvas width={width} height={height} bind:this={canvasRef}></canvas>

<style lang="scss">
    canvas {
        margin: 20px 0;
    }
</style>