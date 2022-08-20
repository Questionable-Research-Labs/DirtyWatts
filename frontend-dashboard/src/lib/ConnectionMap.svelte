<script lang="ts">
    import { Map, Marker, controls } from "@beyonk/svelte-mapbox";
    import { onMount } from "svelte";
    import { getConnectionPoints, type ConnectionPoint } from "./api";
import MapIcon from "./MapIcon.svelte";
    const { GeolocateControl, NavigationControl, ScaleControl } = controls;

    let mapComponent: any;

    // Define this to handle `eventname` events - see [GeoLocate Events](https://docs.mapbox.com/mapbox-gl-js/api/markers/#geolocatecontrol-events)
    function eventHandler(e) {
        const data = e.detail;
        console.log(data);
        // do something with `data`, it's the result returned from the mapbox event
    }

    // Public key for Mapbox
    const ACCESS_TOKEN =
        "pk.eyJ1IjoiZmFsbHN0b3AiLCJhIjoiY2w3MjkyYTFvMHZ6eDN2bXF5aGRveW50cyJ9.0Grfc0Ws4WVdgsGeiXEwrA";

    let connection_points: ConnectionPoint[] = [];

    onMount(async () => {
        if (mapComponent) {
            // Usage of methods like setCenter and flyto
            mapComponent.setCenter([174.1148181731923, -40.8804662625221]); // zoom is optional
            mapComponent.flyTo({
                center: [174.1148181731923, -40.8804662625221],
                zoom: 4,
            }); // documentation (https://docs.mapbox.com/mapbox-gl-js/example/flyto)
        }
        connection_points = await getConnectionPoints();
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
            {#each connection_points as point}
                {#if point.generation_mw > 0 || point.load_mw > 0}
                    <Marker
                        lat={point.latitude}
                        lng={point.longitude}
                        label={`${point.load_mw > 0 ? `Using ${point.load_mw} MW`:""}${point.load_mw > 0 && point.generation_mw > 0 ? " && ": ""}${point.generation_mw > 0 ? `Generating: ${point.generation_mw} MW`:""}`}
                        popupClassName="class-name"
                    >
                        <MapIcon generateMW={point.generation_mw} loadMW={point.load_mw} />
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
</style>
