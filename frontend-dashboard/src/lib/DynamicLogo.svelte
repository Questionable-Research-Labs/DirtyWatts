<script lang="ts">
    import type { PowerStationsResponse } from "./api";
    import { calculateLightColour } from "./indicatorLightColor";



    export let powerStationData: PowerStationsResponse | null;

    // Start with the background grey as a backup
    let logoColor = [51,51,51];
    $: {
        if (powerStationData) {
            logoColor = calculateLightColour(powerStationData);
        }
    }
    let currentWidth = 0;
</script>
<div bind:clientWidth={currentWidth} style="--current-width: {currentWidth}px;">
    <svg viewBox="0 0 1000 1000" fill="currentColor" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" style={`--color: ${logoColor.join(", ")}`} class:loaded={!!powerStationData}>
        <circle cx="537.129" cy="460.822" r="20"/>
        <path class="arm" d="M570.999 499.933C570.999 499.933 577.707 490.971 580.44 485.854C583.26 480.574 585.993 473 585.993 473H640.417L753.153 571.55L809.243 620.519L838.121 646.228C838.121 646.228 848.326 654.359 854.781 659.695C857.002 661.531 857.336 661.531 858.669 663.979C860.755 667.813 860.1 670.445 858.669 672.549C857.002 674.997 853.655 674.914 850.338 674.997C846.595 675.091 840.897 672.549 840.897 672.549L745.933 611.338L637.085 542.169L570.999 499.933Z" />
        <path class="arm" d="M551.936 413.736C551.936 413.736 540.892 412.503 535.128 412.734C529.18 412.973 521.296 414.432 521.296 414.432L494.423 368.07L523.782 222.752L538.334 150.482L546.255 113.026C546.255 113.026 548.231 100.266 549.647 92.0989C550.135 89.2882 549.97 89.0044 551.425 86.6446C553.701 82.9502 556.296 82.1917 558.818 82.3591C561.753 82.5539 563.334 85.447 565.044 88.2309C566.973 91.3724 567.593 97.4977 567.593 97.4977L561.673 209.005L555.743 336.319L551.936 413.736Z" />
        <path class="arm" d="M489.943 472.481C489.943 472.481 493.983 483.457 496.898 488.645C499.907 493.998 504.95 500.386 504.95 500.386L474.757 553.938L326.867 616.746L253.34 648.027L215.055 663.889C215.055 663.889 202.352 669.961 194.15 673.707C191.327 674.996 191.142 675.324 188.282 675.44C183.805 675.622 181.889 673.691 180.861 671.256C179.665 668.421 181.594 665.168 183.362 661.864C185.357 658.135 190.72 653.77 190.72 653.77L296.414 590.215L416.702 516.885L489.943 472.481Z" />
        <path d="M522.046 506.777C522.046 506.777 531.677 508.803 537.853 508.753C543.842 508.705 552.671 506.777 552.671 506.777C552.671 506.777 573.911 912.313 573.911 914.289C573.911 920.216 573.417 920.216 563.044 920.216C554.364 920.216 536.371 920.216 536.371 920.216C536.371 920.216 521.552 920.216 508.216 920.216C504.659 920.216 501.3 920.216 501.3 914.289C501.301 909.736 522.046 506.777 522.046 506.777Z" />
    </svg>
    
</div>

<style lang="scss">
    svg {
        width: 100%;
        height: 100%;
        color: white;
        circle {
            // color: rgb(var(--color));
            stroke: white;
            stroke-width: 4px;
            $repeated-shadow: null;
            $starting-width: 4;
            $ending-width: 8;
            @for $i from $starting-width through $ending-width {
                $repeated-shadow: $repeated-shadow drop-shadow(0 0 calc(var(--current-width)/$i) rgba(var(--color), 1));
            }
            filter: $repeated-shadow;
        }
        .arm {
            transform-origin: 53.7129% 46.0822%;
            transform: scale(0.9) rotate(-5deg);
            transition: all 0.5s ease-out;
        }

        &.loaded {
            circle {
                animation: growCircle 500ms forwards;
                @keyframes growCircle {
                    0% {
                        r: 20;
                    }
                    100% {
                        r: 33.5888;
                    }
                }
            }
            .arm {
                transform: scale(1) rotate(0deg);
            }
        }
    }
</style>