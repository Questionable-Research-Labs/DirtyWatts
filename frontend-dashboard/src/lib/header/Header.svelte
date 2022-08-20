<script lang="ts">
    import LogoNB from "$assets/img/logo-nb.svg"
    import { page } from "$app/stores";
</script>

<header class="header">
    <img src={LogoNB} alt="Dirty Watts Logo" class="header__logo" width="100" height="100"/>
    <div class="header__block">
        <h1 class="header__title">Dirty Watts</h1>

        <nav class="nav">

            <a href="/" class="nav__link" class:active={$page.url.pathname === '/'}>Home</a>
            <a href="/api" class="nav__link" class:active={$page.url.pathname === '/api'}>API</a>
            <a href="https://wiki.dirtywatts.nz/" rel="noreferrer nooppener" class="nav__link">WIKI</a>

        </nav>
    </div>
</header>

<style lang="scss">
  .header {
    display: flex;
    justify-content: left;
    align-items: center;


    background: #333;
    margin-bottom: 1.5rem;
    
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
    text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.2);
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
  }

  .active.nav__link {
    background: #d5d5d5;
    color: #111;
  }

  .nav__link:hover {
    background: #646464;
  }

  .active.nav__link:hover {
    background: #ffffff;
  }
</style>
