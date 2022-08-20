FROM rust as builder
WORKDIR /app
RUN apt update && apt install libpq-dev libssl-dev -y
COPY dirtywatts-scraper/ .
RUN cargo build --release

FROM debian:bullseye
WORKDIR /app
RUN apt update && apt install libpq-dev openssl ca-certificates -y
COPY --from=builder /app/target/release/dirtywatts-scraper ./dirtywatts-scraper
CMD ["./dirtywatts-scraper"]