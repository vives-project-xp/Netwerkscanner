FROM python:3.11-slim

WORKDIR /app

COPY api/ /app

RUN pip install flask mysql-connector-python flask-cors

EXPOSE 80

CMD ["python", "app.py"]



