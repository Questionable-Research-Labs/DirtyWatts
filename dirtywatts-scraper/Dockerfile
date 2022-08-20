FROM rust:alpine as builder
WORKDIR /app
RUN apk add libpq-dev musl-dev
COPY dirtywatts-scraper/Cargo.toml .
COPY dirtywatts-scraper/Cargo.lock .
RUN mkdir src && touch src/main.rs && echo "fn main() {}" >> src/main.rs && cargo build --release && rm -rf src
COPY dirtywatts-scraper/ .
RUN cargo build --release

FROM alpine
WORKDIR /app
COPY --from=builder /app/target/release/dirtywatts-scraper /usr/bin
CMD ["dirtywatts-scraper"]