<script lang="ts">
  import DynamicLogo from "$lib/DynamicLogo.svelte";
  import Prints from "$assets/img/prints.svg";
  import PowerStations from "$lib/PowerStations.svelte";
  import Footer from "$lib/Footer.svelte";
  import { derived } from "svelte/store";
  import { percentRenewable, powerTypes } from "$lib/stores";
  import CoalPower from "$lib/CoalPower.svelte";
  import type { PowerStationsResponse } from "$lib/api";
  import { fade } from "svelte/transition";

  const description =
    "Ever wanted to see where the power you're using is coming from? Want to make sure you reduce the amount of power you're using when Coal is being burned for power? Dirty Watts is the answer to all these problems.";

  let coalPercent = derived(
    powerTypes,
    (powerTypes: PowerStationsResponse | null) => {
      if (!powerTypes) {
        return 0;
      }
      // get total power from power types
      let total = 0;
      for (const key in powerTypes?.power_types) {
        if (!powerTypes?.power_types[key]) continue;
        const { generation_mw } = powerTypes?.power_types[key];
        total += generation_mw;
      }

      return (powerTypes?.power_types["coal"].generation_mw / total) * 100;
    }
  );
</script>

{#if $percentRenewable !== null}
  <div class="mobile-banner clean-power" transition:fade={{ duration: 200 }}>
    {Math.round($percentRenewable * 100)}% Clean Power
  </div>
{/if}

<div class="heading section section--fit">
  <div class="heading__logo">
    <DynamicLogo powerTypes={$powerTypes?.power_types} />
  </div>
  <div>
    <h1 class="section__title">Dirty Watts</h1>
    {#if $coalPercent > 0}
      <CoalPower
        percent={$coalPercent}
        coalMW={$powerTypes?.power_types["coal"]?.generation_mw || 0}
      />
    {/if}
    <p class="section__text">
      Ever wanted to see where the power you're using is coming from? Want to
      make sure you reduce the amount of power you're using when Coal is being
      burned for power? <b>Dirty Watts</b> is the answer to all these problems. We
      provide helpful tools and API's to easily check where New Zealand's power is
      being generated
    </p>
  </div>
</div>

<svelte:head>
  <title>Dirty Watts - Home</title>
  <meta property="og:title" content="Dirty Watts - Home" />
  <meta name="description" content={description} />
  <meta property="og:description" content={description} />
</svelte:head>

<img
  src={Prints}
  class="prints"
  alt="carbon footprint"
  title="Carbon footprint"
/>

<PowerStations />

<Footer />

<style lang="scss">
  // Force scrollbar so model poup doesn't shift layout
  :global(html) {
    overflow-y: scroll;
  }
  .heading {
    display: flex;
    justify-content: center;
    align-items: center;
  }

  .heading__logo {
    width: 100%;
    max-width: 512px;
    max-height: 512px;
    margin: 1rem auto;
    @media screen and (max-width: 500px) {
      margin: 0 auto;
    }
  }

  .prints {
    margin: 0 auto 6rem;
    display: block;
    width: 100%;
    max-width: 256px;
    @media screen and (max-width: 500px) {
      margin: 3rem auto 3rem;
      max-width: 180px;
    }
  }

  @media all and (max-width: 1000px) {
    .heading {
      flex-flow: column;
    }
  }

  .mobile-banner {
    display: none;
    @media screen and (max-width: 600px) {
      display: block;
      color: #fff;
      font-size: 1.5rem;
      font-weight: bold;
      padding: 0.5rem 0.5rem 0 0.5rem;
      text-align: center;
      border-top: 2px solid white;
    }
  }
</style>
