import sqlite3
from fastapi import FastAPI, Form
from fastapi.responses import HTMLResponse, PlainTextResponse

app = FastAPI()

# --- Database Initialization ---
# check_same_thread=False allows background worker threads to write safely
conn = sqlite3.connect("binance_tracker.db", check_same_thread=False)
cursor = conn.cursor()

# Create table with automated timestamp for historical price tracking
cursor.execute("""
    CREATE TABLE IF NOT EXISTS crypto_prices (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        bitcoin_price TEXT,
        altcoin_price TEXT,
        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
""")
conn.commit()


@app.get("/")
def health_check():
    """Simple endpoint to verify server status."""
    print("Server is active.")
    return {"status": "active", "message": "Binance Telemetry Server is running."}


@app.post("/receive_data")
def receive_crypto_data(BTCUSDT: str = Form(...), SNXUSDT: str = Form(...)):
    """
    Receives form-urlencoded data from the ESP32,
    logs it into the SQLite database, and returns a success payload.
    """
    print(f"Incoming Data -> BTC: {BTCUSDT}, SNX: {SNXUSDT}")

    cursor.execute(
        "INSERT INTO crypto_prices (bitcoin_price, altcoin_price) VALUES (?, ?)",
        (BTCUSDT, SNXUSDT)
    )
    conn.commit()

    return {"status": "success", "message": "Data successfully logged to database!"}


@app.get("/receive_data")
def view_crypto_logs():
    """
    Endpoint for querying historical crypto price logs from the database.
    """
    cursor.execute("SELECT * FROM crypto_prices ORDER BY timestamp DESC")
    records = cursor.fetchall()

    return {
        "total_records": len(records),
        "historical_data": records
    }