FROM tiangolo/uvicorn-gunicorn-fastapi:python3.11-2024-12-02

COPY ./public-api/requirements.txt /app
RUN pip install -r ./requirements.txt

COPY ./public-api/app/ /app
ENV TZ="Pacific/Auckland"
# Runs with defualt port 80
ENV MAX_WORKERS=4