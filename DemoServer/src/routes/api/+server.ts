let store: string;

/** @type {import('./$types').RequestHandler} */
export function GET({ }) {
  return new Response(store);
}2

export async function POST({ request }: { request: Request }) {
  store = await request.text();
  console.log(store)
  return new Response();
}
