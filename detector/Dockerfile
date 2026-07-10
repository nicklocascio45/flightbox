# Official python runtime
FROM python:3.13-slim

WORKDIR /app

# Copy and install requirements first for caching
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

CMD ["python", "-m", "orchestrator"]
