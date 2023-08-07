import type { PowerStationsResponse } from "./api";

export type RGBColor = [number, number, number];

export function cssRGBFormatter(colour: RGBColor) {
	return "rgb(" + colour[0] + "," + colour[1] + "," + colour[2] + ")";
}

export function findClosest(powerTypesHistory: PowerStationsResponse[], previewPosition: number) {
	if (!powerTypesHistory.length) {
		return null;
	}
	powerTypesHistory = powerTypesHistory.sort((a, b) => new Date(a.timestamp).getTime() - new Date(b.timestamp).getTime());
	console.log(powerTypesHistory, previewPosition);
	// Get min and max dates in data
	let minDate = new Date(powerTypesHistory[0].timestamp);
	let maxDate = new Date(powerTypesHistory[powerTypesHistory.length - 1].timestamp);
	// Get interpolated date
	let date = new Date(
		minDate.getTime() +
		(maxDate.getTime() - minDate.getTime()) * previewPosition
	);
	// Find nearest datapoints and interpolate between them
	let index = powerTypesHistory.findIndex(
		(d) => new Date(d.timestamp) > date
	);
	let prev = powerTypesHistory[index - 1];
	let next = powerTypesHistory[index];
	console.log(prev, next, date)
	if (!prev || !next) {
		return powerTypesHistory[powerTypesHistory.length - 1];
	}

	let prevDate = new Date(prev.timestamp);
	let nextDate = new Date(next.timestamp);
	let nextWeight = (date.getTime() - prevDate.getTime()) / (nextDate.getTime() - prevDate.getTime());
	if (nextWeight > 0.5) {
		return next;
	} else {
		return prev;
	}
}