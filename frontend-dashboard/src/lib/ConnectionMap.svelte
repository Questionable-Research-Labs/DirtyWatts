<script lang="ts">
    import { Map, Marker, controls } from "@beyonk/svelte-mapbox";
    import { onMount } from "svelte";
    const { GeolocateControl, NavigationControl, ScaleControl } = controls;

    let mapComponent: any;
    onMount(()=>{
        if (mapComponent) {
            // Usage of methods like setCenter and flyto
            mapComponent.setCenter([174.1148181731923, -40.8804662625221]); // zoom is optional
            mapComponent.flyTo({ center: [174.1148181731923, -40.8804662625221], zoom: 4 }); // documentation (https://docs.mapbox.com/mapbox-gl-js/example/flyto)
        }
    });

    // Define this to handle `eventname` events - see [GeoLocate Events](https://docs.mapbox.com/mapbox-gl-js/api/markers/#geolocatecontrol-events)
    function eventHandler(e) {
        const data = e.detail;
        console.log(data);
        // do something with `data`, it's the result returned from the mapbox event
    }

    // Public key for Mapbox
    const ACCESS_TOKEN = "pk.eyJ1IjoiZmFsbHN0b3AiLCJhIjoiY2w3MjkyYTFvMHZ6eDN2bXF5aGRveW50cyJ9.0Grfc0Ws4WVdgsGeiXEwrA";
</script>

<section class="map-container">
    <div class="map-wrapper">
        <Map
        accessToken={ACCESS_TOKEN}
        bind:this={mapComponent}
        options={{ scrollZoom: true, style: "mapbox://styles/mapbox/dark-v10" }}
    >
        <!-- <Marker
            lat={20}
            lng={32}
            color={"rgb(255,0,255)"}
            label="some marker label"
            popupClassName="class-name"
        /> -->
        <NavigationControl />
        <GeolocateControl
            options={{ some: "control-option" }}
            on:eventname={eventHandler}
        />
        <ScaleControl />
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