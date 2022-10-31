# stage build
FROM node:16-alpine

WORKDIR /app

# copy everything to the container
COPY DemoServer/. .

# clean install all dependencies
RUN yarn install --frozen-lockfile
    
# build SvelteKit app
RUN yarn build


# stage run
FROM node:16-alpine

WORKDIR /app

# copy dependency list
COPY --from=0 /app/package*.json ./
COPY --from=0 /app/yarn.lock ./

# clean install dependencies, no devDependencies, no prepare script
RUN yarn install --production --ignore-scripts --prefer-offline

# copy built SvelteKit app to /app
COPY --from=0 /app/build ./

EXPOSE 3000
CMD ["node", "./index.js"]
