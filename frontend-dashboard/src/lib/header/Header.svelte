<script lang="ts">
    import Logo from "$assets/img/logo.svg"
</script>

<header class="header">
    <img src={Logo} alt="Dirty Watts Logo" class="header__logo" width="128" height="128"/>
    <h1 class="header__title">Dirty Watts</h1>
</header>

<style lang="scss">
  .header {
    display: flex;
    justify-content: left;
    align-items: center;


    background: #333;
  }

  .header__logo {
    margin-left: 0;
    margin-right: 1rem;
    max-height: 120px;
    width: auto;
  }

  .header__title {
    color: white;
    font-weight: bold;
    letter-spacing: 2px;
    text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.2);
    text-transform: uppercase;
  }
</style>
