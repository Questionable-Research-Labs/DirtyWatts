<script lang="ts">
    import { Map, Marker, controls } from "@beyonk/svelte-mapbox";
    import { onMount } from "svelte";
    import type { ConnectionPoint } from "./api";
    import ConnectionPointHistory from "./ConnectionPointHistory.svelte";
    import MapIcon from "./MapIcon.svelte";

    import { getContext } from "svelte";
    import { connectionPoints } from "./stores";
    const { open } = getContext("simple-modal");

    const { GeolocateControl, NavigationControl, ScaleControl } = controls;

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

<section class="section">
    <h1 class="section__title">Grid Connection Map</h1>
    <p></p>
</section>
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
            <NavigationControl />
            <!-- <ScaleControl /> -->
        </Map>
    </div>
</section>

<style lang="scss">
    .map-container {
        width: 100%;
        max-width: 1000px;
        margin: 1rem auto;
        position: relative;
        padding-bottom: 56.25%; /* 16:9 */
        height: 0;
        overflow: show;

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
