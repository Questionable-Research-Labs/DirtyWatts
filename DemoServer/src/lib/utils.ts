export type RGBColor = [number, number, number];

export function cssRGBFormatter(colour: RGBColor) {
  return "rgb(" + colour[0] + "," + colour[1] + "," + colour[2] + ")";
}
