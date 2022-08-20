<script lang="ts">
	import Header from '$lib/header/Header.svelte';
	import '$assets/scss/app.scss';
</script>

<Header />

<main>
	<slot />
</main>

<footer>
</footer>

<style>

	main {
		max-width: 1000px;
		margin: 1rem auto;
	}

</style>
