<script lang="ts">
    import { browser } from "$app/environment";
    import { generateFakeData } from "$lib/calculatePowerstationData";
	import RangeSlider from "svelte-range-slider-pips";
    import { derived, writable } from "svelte/store";

	let sliderValue = writable([50]);
	if (browser) {
		setInterval(() => {
		fetch("/api", {
			method: 'POST',
			body: JSON.stringify(generateFakeData($sliderValue[0]))
		});
	}, 1000);
	}

	function colorChannelMixer(colorChannelA: number, colorChannelB: number, amountToMix: number){
		var channelA = colorChannelA*amountToMix;
		var channelB = colorChannelB*(1-amountToMix);
		return Math.round(channelA+channelB);
	}
	type RGBColor = [number, number, number];
	function colorMixer(rgbA: RGBColor, rgbB: RGBColor, amountToMix: number){
		var r = colorChannelMixer(rgbA[0],rgbB[0],amountToMix);
		var g = colorChannelMixer(rgbA[1],rgbB[1],amountToMix);
		var b = colorChannelMixer(rgbA[2],rgbB[2],amountToMix);
		return "rgb("+r+","+g+","+b+")";
	}

	let highlightColour = derived(sliderValue, (value) => {
		return colorMixer([203, 95, 95], [19, 148, 79], value[0]/100)
	});

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
		suffix="%"
		bind:values={$sliderValue}
	/>
	<h2>{$sliderValue[0]}%</h2>

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
		}
		h2 {
			font-size: 8rem;
			text-align: center;
			color: var(--highlight-color);
			text-shadow: -1px -1px 0 #000, 1px -1px 0 #000, -1px 1px 0 #000, 1px 1px 0 #000;
			filter: drop-shadow(0 0 0.75rem var(--highlight-color));
		}
	}
</style>