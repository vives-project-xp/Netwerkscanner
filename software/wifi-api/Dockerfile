FROM python:3.11-slim

WORKDIR /app


COPY requirements.txt .  


RUN pip install --no-cache-dir -r requirements.txt

COPY . .

EXPOSE 80

# Start de app vanuit de api map
CMD ["python", "api/app.py"]