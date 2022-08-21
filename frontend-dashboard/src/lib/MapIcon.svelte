<script lang="ts">
    export let loadMW: number;
    export let generateMW: number;

    $: majorityClass = generateMW >= loadMW ? "generate" : "load";
</script>
<div class={`icon ${majorityClass}`} style={`--size: ${Math.max(Math.log(generateMW + loadMW) * 10, 16)}px`}></div>

<style lang="scss">
    .icon {
        width: var(--size);
        height: var(--size);
        border-radius: calc(var(--size) / 2);
        cursor: pointer;
        border: 1px solid #ccc;
        
        
        &.generate {
            background-color: #00b30044;

        }
        &.load {
            background-color: #ff000044;
        }
    }
</style>