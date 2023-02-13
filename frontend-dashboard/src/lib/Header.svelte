<script lang="ts">
  import LogoNB from "$assets/img/logo-nb.svg";
  import { page } from "$app/stores";
  import { percentRenewable } from "./stores";
  import { fade } from "svelte/transition";
</script>

<header
  class="header {$percentRenewable != null ? 'fancy-progress' : ''}"
  style="--percent-renewable: {$percentRenewable || 1};"
>
  <img
    src={LogoNB}
    alt="Dirty Watts Logo"
    class="header__logo"
    width="100"
    height="100"
  />
  <div class="header__block">
    <h1 class="header__title">Dirty Watts</h1>

    <nav class="nav">
      <a href="/" class="nav__link" class:active={$page.url.pathname === "/"}
        >Home</a
      >
      <a
        href="/map"
        class="nav__link"
        class:active={$page.url.pathname === "/map"}>MAP</a
      >
      <a
        href="https://api.dirtywatts.nz/"
        rel="noreferrer nooppener"
        class="nav__link"
        target="_blank">API</a
      >
      <a
        href="https://wiki.dirtywatts.nz/"
        rel="noreferrer nooppener"
        class="nav__link"
        target="_blank">WIKI</a
      >
    </nav>
  </div>
  {#if $percentRenewable !== null}
    <div class="power-percent clean-power" transition:fade={{ duration: 200 }} title={`${($percentRenewable*100).toPrecision(3)}% Renewable Energy`}>
      {Math.round($percentRenewable * 100)}% Clean
    </div>
    {#if $percentRenewable <= 0.95}
      <div
        class="power-percent dirty-power"
        transition:fade={{ duration: 200 }}
      >
        {Math.round((1 - $percentRenewable) * 100)}% Dirty
      </div>
    {/if}


  {/if}
</header>

<style lang="scss">
  .header {
    display: flex;
    justify-content: left;
    align-items: center;
    position: relative;
    transition: all 0.2s ease-in-out;

    background: #333;

    &.fancy-progress {
      background: linear-gradient(
        90deg,
        rgba(51, 51, 51, 1) 0%,
        rgba(51, 51, 51, 1) 33%,
        rgb(19, 148, 79) calc(calc(var(--percent-renewable) * 100%) - 1px),
        rgb(255, 255, 255) calc(calc(var(--percent-renewable) * 100%) - 1px),
        rgb(255, 255, 255) calc(calc(var(--percent-renewable) * 100%) + 1px),
        rgb(203, 95, 95) calc(calc(var(--percent-renewable) * 100%) + 1px)
      );
    }

    .power-percent {
      position: fixed;
      color: #fff;
      font-size: 1.5rem;
      font-weight: bold;
      padding: 0.5rem;
      background: #333;
      box-sizing: content-box;

      &.clean-power {
        right: calc(calc(1 - var(--percent-renewable)) * 100%);
        text-align: right;
        border-right: 1px solid white;
      }

      &.dirty-power {
        left: calc(var(--percent-renewable) * 100%);
        text-align: left;
        border-left: 1px solid white;
      }

      @media screen and (max-width: 1500px) {
        position: absolute;
        &.dirty-power {
          display: none;
        }
      }
      @media screen and (max-width: 600px) {
        &.clean-power {
          display: none;
        }
      }
    }
  }

  .header__logo {
    margin-left: 0;
    margin-right: 1rem;
    max-height: 120px;
    width: auto;
  }

  .header__block {
    display: flex;
    flex-flow: column;
    justify-content: space-between;
    gap: 0.25rem;
  }

  .header__title {
    color: white;
    font-weight: 900;
    letter-spacing: 2px;
    text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);
    text-transform: uppercase;
  }

  .nav {
    display: flex;
    flex-flow: row;
    gap: 0.5rem;
  }

  .nav__link {
    color: #cecece;
    text-transform: uppercase;
    font-weight: bold;
    text-decoration: none;
    padding: 0.5rem;

    transition: color 0.5s ease, background-color 0.25s ease, transform 0.2s;

    &:hover {
      background: #646464;
    }
    &.active {
      background: #d5d5d5;
      color: #111;
      &:hover {
        background: #ffffff;
      }
    }
  }
</style>
