<script lang="ts">
    import { browser } from "$app/environment";
    import { calculateLightColour } from "$lib/calculateLightColour";
    import { generateFakeData } from "$lib/calculatePowerstationData";
    import type { PowerTypes, PowerTypesResponse } from "$lib/powerstation";
    import { onMount } from "svelte";
	import RangeSlider from "svelte-range-slider-pips";
    import { derived, writable } from "svelte/store";
	
	import "../app.scss";

	let sliderValue = writable([50]);

	function cssRGBFormatter(colour: RGBColor) {
		return "rgb("+colour[0]+","+colour[1]+","+colour[2]+")";
	}

	let fakeData = derived(sliderValue, (value)=>generateFakeData(100-value[0]))

	let highlightColour = derived(fakeData, (data) => {
		return cssRGBFormatter(calculateLightColour(data.power_types))
	});

	function calculatePercentRenewable(data: PowerTypesResponse) {
		const power_types = data.power_types;
		let total_generation_mw = 0   // total up power usage
		for(let key in power_types){
			let power_type = power_types[key];
			total_generation_mw += power_type.generation_mw;
		}

		if (total_generation_mw === 0) {
			console.log("No generation?!")
			return -1
		}
		const bad_generation_mw = power_types.coal.generation_mw + power_types.gas.generation_mw + power_types.diesel.generation_mw // + power_types.co_gen.generation_mw
		
		return ((total_generation_mw - bad_generation_mw) / total_generation_mw)  // calculate % non-renewable

	}

	let percentRenewable = derived(fakeData, calculatePercentRenewable)

	let serverState: PowerTypesResponse | {timestamp: ""} = {timestamp: ""};
	onMount(()=>{
		if (browser) {
			setInterval(() => {
				if ($fakeData.timestamp != serverState.timestamp) {
					fetch("/api", {
						method: 'POST',
						body: JSON.stringify($fakeData)
					});
					serverState = $fakeData
				}
			}, 500);
		}
	})
	
	function pipFormatter(value: number, pipIndex: number, percent: number): string {
		return Math.round(calculatePercentRenewable(generateFakeData(100-value)) * 100) + "%"
	}

</script>

<svelte:head>
	<title>Home</title>
	<meta name="description" content="Svelte demo app" />
</svelte:head>

<div class="container" style="--highlight-color: {$highlightColour};">
	<h1>Dirty Watts Demo</h1>

	<div class="slider-wrapper">
		<RangeSlider
		min={0}
		max={100}
		step={1}
		pips={true}
		float
		pipstep=10
		first="label"
		last="label"
		rest="label"
		formatter={pipFormatter}
		bind:values={$sliderValue}
	/>
	<h2>{Math.round($percentRenewable * 100)}%</h2>
	<h3>Percent Renewable</h3>

	</div>
</div>

<style lang="scss">
	@import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@500;800&display=swap');
	:global(body) {
		font-family: 'JetBrains Mono', monospace;
		background-color: #333;
	}

	.container {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
		height: 100vh;
		.slider-wrapper {
			min-width: 80vw;
		}
		h1 {
			font-weight: bold;
			color: white;
			font-size: 6rem;
		}
		h2 {
			font-size: 8rem;
			text-align: center;
			color: var(--highlight-color);
			text-shadow: -1px -1px 0 #fff, 1px -1px 0 #fff, -1px 1px 0 #fff, 1px 1px 0 #fff;
			filter: drop-shadow(0 0 0.75rem var(--highlight-color));
			margin: 0;
		}
		h3 {
			text-align: center;
			color: white;
			margin: 0;
		}
	}
</style>