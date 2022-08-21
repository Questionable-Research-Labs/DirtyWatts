<script lang="ts">
    import CoalIcon from "$assets/img/coal.png"

    export let percent: number;
    export let coalMW: number;
</script>

<div class="coal">
    <img src={CoalIcon} alt="Coal Icon" class="coal__icon" width="64" height="64">
    <div class="coal__block">
        <h1 class="coal__title">Coal Power In Use</h1>
        <p class="coal__text">Currently {Math.ceil(percent)}% of power is coming from Coal ({Math.ceil(coalMW)} MW)</p>
    </div>
</div>

<style lang="scss">

.coal {
  padding: 1rem;
  display: flex;
  max-width: 600px;

  align-items: center;
  gap: 2rem;

  margin: 1rem auto 2rem;

  border-radius: 5px;

  background: #462a2a;
  border: 5px solid #d96c6c;

  box-shadow: 2px 2px 10px rgba(0,0,0,1);
}

.coal__block {
  display: flex;
  flex-flow: column;
}

.coal__title {
  color: white;
}

.coal__text {
  color: #ECECEC;
}

</style>