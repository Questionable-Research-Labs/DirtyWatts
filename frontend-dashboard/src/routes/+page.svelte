<script lang="ts">
    import Logo from "$assets/img/logo-nb.svg"
    import Prints from "$assets/img/prints.svg"
    import PowerStations from "$lib/PowerStations.svelte";
    import ConnectionMap from "$lib/ConnectionMap.svelte";
</script>

<div class="heading section section--fit">
    <img class="heading__logo" src={Logo} width="512" height="512" alt="Dirty Watts Logo"/>
    <div>
        <h1 class="section__title">Dirty Watts</h1>
        <p class="section__text">
            Ever wanted to see where the power you're using is coming from? Want to make sure you reduce the amount of
            power you're using when Coal is being burned for power? <b>Dirty Watts</b> is the answer to all these problems.
            We provide helpful tools and APIS to easily check where New Zealand's power is being generated
        </p>
    </div>
</div>

<img src={Prints} class="prints">

<PowerStations/>

<style lang="scss">
  // Force scrollbar so model poup doesn't shift layout
  :global(html) {
    overflow-y: scroll;
  }
  .heading {
    display: flex;
    justify-content: center;
    align-items: center;
  }


  .heading__logo {
    width: 100%;
    max-width: 512px;
    margin: 1rem auto;
  }


  .prints {
    margin: 0 auto 6rem;
    display: block;
    width: 100%;
    max-width: 256px;
  }

  @media all and (max-width: 1000px) {
    .heading {
      flex-flow: column;
    }
  }

</style>