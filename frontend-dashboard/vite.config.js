import { sveltekit } from '@sveltejs/kit/vite';
import { resolve } from "path";

/** @type {import('vite').UserConfig} */
const config = {
	plugins: [sveltekit()],
	resolve: {
		alias: {
			$assets: resolve('./src/assets'),
		}
	}
};

export default config;
