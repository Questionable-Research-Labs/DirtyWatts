<script lang="ts">
    export let height = 128;
</script>
<div class="container" style="--height: {height}px;">
    <span class="spinner" />
</div>

<style lang="scss">
    .container {
        display: flex;
        justify-content: center;
        align-items: center;
        height: var(--height);

        .spinner {
            display: block;
            width: 128px;
            height: 128px;
            border: 15px solid transparent;
            border-top-color: #00d241;
            border-radius: 100%;
            animation: 1s linear infinite loader;
        }

    }

    @keyframes loader {
        0% {
            transform: rotate(0deg);
        }
        100% {
            transform: rotate(360deg);
        }
    }
</style>