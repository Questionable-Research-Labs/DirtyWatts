<script lang="ts">
	import { Datepicker, themes } from 'svelte-calendar';
    import {calendarDate} from "$lib/api";
    const { dark: theme } = themes;

    function getDateFromDatetime(datetime: Date): string {
        return datetime.toISOString().split("T")[0];
    }
</script>
<Datepicker {theme} bind:selected={$calendarDate} format={"YYYY-MM-DD"} end={new Date()} start={new Date("2022-08-20")}  />
<button class="latest-button" class:latest-data={getDateFromDatetime($calendarDate)===getDateFromDatetime(new Date())} on:click={()=>{calendarDate.set(new Date())}}>Latest</button>

<style lang="scss">
    .latest-button {
        padding: 16px 30px;
        background: var(--sc-theme-calendar-colors-background-primary);
        color: var(--sc-theme-calendar-colors-text-primary);
        font-size: 1.3em;
        border-radius: 2px;
        border: 0;
        box-shadow: 4px 3px 9px rgb(0 0 0 / 20%);
        cursor: pointer;
        height: 54px;
        margin-left: 1rem;
        outline: gray solid 1px;
        &.latest-data {
            outline: white solid 1px;
        }
    }

    :global(button) {
        outline: white solid 1px;
    }
</style>