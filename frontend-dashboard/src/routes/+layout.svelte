<script lang="ts">
    import Header from "$lib/header/Header.svelte";
    import "$assets/scss/app.scss";
    import Modal from "svelte-simple-modal";
    import { onMount } from "svelte";
    import { initialiseAPI } from "$lib/api";
    import Footer from "$lib/Footer.svelte";

    onMount(async () => {
        await initialiseAPI();
    });
</script>

<Modal classWindow={"modal-window"}>
    <Header/>

    <slot/>

</Modal>

<style lang="scss">

  :global(.window.modal-window) {
    background-color: #333;
    padding-top: 2em;
  }

  :global(.window.modal-window .close) {
    background-color: #333;
    color: white;
    box-shadow: 0 0 0 1px white;
    cursor: pointer;
  }

  :global(.window.modal-window .close::before, .window.modal-window
			.close::after) {
    background-color: white;
    transition: all 0.2s ease-in-out;
  }

  :global(.window.modal-window .close:hover) {
    background-color: white;
    color: #333;
    box-shadow: 0 0 0 1px #333;
  }

  :global(.window.modal-window .close:hover::before, .window.modal-window
			.close:hover::after) {
    background-color: #333;
    transition: all 0.2s ease-in-out;
  }
</style>
