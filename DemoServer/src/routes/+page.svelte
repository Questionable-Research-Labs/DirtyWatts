<script lang="ts">
  import { calculateLightColourOld } from "$lib/calculateLightColour";
  import { onMount } from "svelte";
  import { derived, writable } from "svelte/store";
  import {cssRGBFormatter} from "$lib/utils";

  import "../app.scss";
    import PowerStations from "$lib/PowerStations.svelte";
    import { initialiseAPI, type PowerStationsResponse } from "$lib/api";
    import { lightColour, newLightColorSystem, percentRenewable, previewDatapoint } from "$lib/stores";
    import Switch from "$lib/Switch.svelte";
    import ColourPreviewBar from "$lib/ColourPreviewBar.svelte";


  onMount(async () => {
		await initialiseAPI();
	});

</script>

<svelte:head>
  <title>Home</title>
  <meta name="description" content="Svelte demo app" />
</svelte:head>

<div class="container" style="--highlight-color: {cssRGBFormatter($lightColour)};">
  <div class="switch-container">
    <span class="switch-label">New Color Algorithm</span>
    <Switch bind:checked={$newLightColorSystem} scale={1.5} />
  </div>
  <h1>DirtyWatts Demo</h1>
  <PowerStations />
  <div class="slider-wrapper">
    <h2>{#if $percentRenewable}{Math.round($percentRenewable * 100)}{:else}--{/if}%</h2>
    <h3>Percent Renewable</h3>
  </div>
</div>

<style lang="scss">
  @import url("https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@500;800&display=swap");
  :global(body) {
    font-family: "JetBrains Mono", monospace;
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
      margin: 2rem auto;
    }
    h2 {
      font-size: 8rem;
      text-align: center;
      color: var(--highlight-color);
      text-shadow: -1px -1px 0 #fff, 1px -1px 0 #fff, -1px 1px 0 #fff,
        1px 1px 0 #fff;
      filter: drop-shadow(0 0 0.75rem var(--highlight-color));
      margin: 0;
    }
    h3 {
      text-align: center;
      color: white;
      margin: 0;
    }
    .switch-container {
      position: absolute;
      top: 1em;
      right: 1em;
      width: max-content;
      display: flex;
      align-items: center;
      justify-content: center;
      margin-bottom: 2rem;
      .switch-label {
        color: white;
        margin-right: 1rem;
      }
    }
  }
</style>
