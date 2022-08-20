<script lang="ts">
    import { controls, Map, Marker } from "@beyonk/svelte-mapbox";
    import { getContext, onMount } from "svelte";
    import ConnectionPointHistory from "$lib/ConnectionPointHistory.svelte";
    import MapIcon from "$lib/MapIcon.svelte";
    import { connectionPoints } from "$lib/stores";

    const {open} = getContext("simple-modal");

    const {GeolocateControl, NavigationControl} = controls;

    let mapComponent: any;

    // Public key for Mapbox
    const ACCESS_TOKEN =
        "pk.eyJ1IjoiZmFsbHN0b3AiLCJhIjoiY2w3MjkyYTFvMHZ6eDN2bXF5aGRveW50cyJ9.0Grfc0Ws4WVdgsGeiXEwrA";

    onMount(async () => {
        if (mapComponent) {
            mapComponent.setCenter([174.1148181731923, -40.8804662625221]); // zoom is optional
            mapComponent.flyTo({
                center: [174.1148181731923, -40.8804662625221],
                zoom: 4,
            }); // documentation (https://docs.mapbox.com/mapbox-gl-js/example/flyto)
        }
    });
</script>


<section class="map-container">
    <div class="map-wrapper">
        <Map
                accessToken={ACCESS_TOKEN}
                bind:this={mapComponent}
                options={{
                scrollZoom: true,
                style: "mapbox://styles/mapbox/dark-v10",
            }}
        >
            {#each $connectionPoints as point}
                {#if point.generation_mw > 0 || point.load_mw > 0}
                    <Marker
                            lat={point.latitude}
                            lng={point.longitude}
                            popupClassName="class-name"
                    >
                        <MapIcon
                                generateMW={point.generation_mw}
                                loadMW={point.load_mw}
                        />
                        <div slot="popup" class="popup">
                            <span class="popup-header">{point.address}</span>
                            <div class="popup-content">
                                <span class="popup-content-label">Load:</span>
                                <span class="popup-content-value"
                                >{point.load_mw.toFixed(1)} MW</span
                                >
                            </div>
                            <div class="popup-content">
                                <span class="popup-content-label"
                                >Generation:</span
                                >
                                <span class="popup-content-value"
                                >{point.generation_mw.toFixed(1)} MW</span
                                >
                            </div>
                            <button
                                    class="popup-more-info"
                                    on:click={() => {
                                    open(ConnectionPointHistory, {
                                        connectionPointID:
                                            point.connection_code,
                                    });
                                }}
                            >
                                View History
                            </button>
                        </div>
                    </Marker>
                {/if}
            {/each}
            <NavigationControl/>
            <!-- <ScaleControl /> -->
        </Map>
    </div>
</section>

<style lang="scss">
  // Force scrollbar so model poup doesn't shift layout
  :global(html) {
    overflow-y: scroll;
  }

  .map-container {
    display: flex;
    flex-flow: column;


    width: 100%;
    position: relative;
    height: calc(100vh - 100px - 2rem);

    .map-wrapper {
      position: absolute;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
    }
  }

  .popup {
    .popup-header {
      font-size: 1rem;
      font-weight: 900;
      letter-spacing: 0.1rem;
    }

    .popup-content-label {
      font-size: 0.8rem;
      font-weight: 900;
    }

    .popup-more-info {
      min-width: 100%;
      font-size: 0.8rem;
      font-weight: 900;
      color: #fff;
      background: #333;
      padding: 0.5em;
      border: none;
      cursor: pointer;
    }
  }
</style>