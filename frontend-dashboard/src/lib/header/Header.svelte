<script lang="ts">
    import LogoNB from "$assets/img/logo-nb.svg"
</script>

<header class="header">
    <img src={LogoNB} alt="Dirty Watts Logo" class="header__logo" width="100" height="100"/>
    <h1 class="header__title">Dirty Watts</h1>
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

  .header__title {
    color: white;
    font-weight: 900;
    letter-spacing: 2px;
    text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.2);
    text-transform: uppercase;
  }
</style>
