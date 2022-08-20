<script lang="ts">
    import QRL from "$assets/img/qrl.svg"
    import { page } from "$app/stores";
</script>

<footer class="footer">
    <p class="footer__text">Service Provided By</p>
    <img src={QRL} alt="QRL Logo" class="footer__logo" width="256" height="256"/>
</footer>

<style lang="scss">
  .footer {
    display: flex;
    width: 100%;
    justify-content: center;
    align-items: center;
    flex-flow: column;
    margin-top: 15vh;
  }


  .footer__text {
    color: #999;
  }

  .footer__logo {
        margin: 1rem auto;
  }
</style>
