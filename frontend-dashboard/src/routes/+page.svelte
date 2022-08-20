<script lang="ts">
    import ConnectionMap from "$lib/ConnectionMap.svelte";
    import PowerStations from "$lib/PowerStations.svelte";
</script>

<svelte:head>
    <title>Home</title>
    <meta name="description" content="Svelte demo app"/>
</svelte:head>


<PowerStations/>

<section class="section">
    <h1 class="section__title">Grid Connection Map</h1>
</section>
<ConnectionMap/>


<section class="section">
    <h1 class="section__title">About</h1>
</section>

<style lang="scss">
    // Force scrollbar so model poup doesn't shift layout
    :global(html) {
        overflow-y: scroll;
    }
</style>
