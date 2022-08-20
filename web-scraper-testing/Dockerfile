FROM rust:alpine as builder
WORKDIR /app
RUN apk add libpq-dev musl-dev
COPY . .
RUN cargo build --release

FROM alpine
WORKDIR /app
COPY --from=builder /app/target/release/dirtywatts-scraper /usr/bin
CMD ["dirtywatts-scraper"]