import psutil
import GPUtil
import sqlite3
from fastapi import FastAPI
from fastapi.responses import HTMLResponse, PlainTextResponse

app = FastAPI()

# --- Database Initialization ---
# check_same_thread=False allows asynchronous workers to write to the DB without locking
conn = sqlite3.connect('pc_telemetry.db', check_same_thread=False)
cursor = conn.cursor()

# Create table with automatic timestamp for historical performance tracking
cursor.execute('''
    CREATE TABLE IF NOT EXISTS performance_logs (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        cpu_usage REAL,
        gpu_temp REAL,
        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
''')
conn.commit()


@app.get("/")
def health_check():
    """Simple endpoint to verify server and database status."""
    return {"status": "Database and Server are active!"}


@app.get("/cpu_gpu_telemetry")
def fetch_telemetry():
    """
    Reads hardware sensors, logs the data to SQLite,
    and returns a JSON response for the ESP32 to parse.
    """
    # Fetch hardware metrics
    cpu_val = psutil.cpu_percent(interval=None)

    # GPUtil returns a list of GPUs. We pull the temperature of the primary GPU (index 0).
    gpus = GPUtil.getGPUs()
    gpu_val = gpus[0].temperature if gpus else 0.0

    # Log metrics to DB
    cursor.execute("INSERT INTO performance_logs (cpu_usage, gpu_temp) VALUES (?, ?)", (cpu_val, gpu_val))
    conn.commit()

    # JSON response payload for the ESP32
    return {"cpu": cpu_val, "gpu": gpu_val}


@app.get("/view_logs")
def get_historical_logs():
    """
    Endpoint for debugging and reviewing past performance telemetry.
    """
    cursor.execute("SELECT * FROM performance_logs ORDER BY timestamp DESC")
    records = cursor.fetchall()

    return {
        "total_records": len(records),
        "history": records
    }