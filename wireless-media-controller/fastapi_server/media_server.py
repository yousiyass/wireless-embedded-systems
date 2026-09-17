import sqlite3
from fastapi import FastAPI, Form
import keyboard

app = FastAPI()

# --- Database Initialization ---
# check_same_thread=False allows FastAPI's asynchronous workers to access the SQLite DB concurrently
conn = sqlite3.connect("command_logs.db", check_same_thread=False)
cursor = conn.cursor()

# Create table with automatic timestamp generation for auditing
cursor.execute("""
        CREATE TABLE IF NOT EXISTS access_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            executed_action TEXT,
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
        """)
conn.commit()


@app.get("/")
def health_check():
    # Simple endpoint to verify server status from a browser
    print("Server is active and listening.")
    return {"status": "online"}


@app.post("/receive_command")
def process_incoming_command(command: str = Form(...)):
    """
    Receives HTTP POST requests from the ESP32.
    Logs the event to SQLite and triggers the physical keyboard action.
    """
    print(f"Incoming Command: {command}")

    # Log to DB
    cursor.execute("INSERT INTO access_logs (executed_action) VALUES (?)", (command,))
    conn.commit()

    # Hardware execution routing
    if command == "vol_up":
        keyboard.send("volume up")
    elif command == "vol_down":
        keyboard.send("volume down")
    elif command == "mute":
        keyboard.send("volume mute")
    elif command == "next_track":
        keyboard.send("next track")
    elif command == "prev_track":
        keyboard.send("previous track")
    elif command == "play_pause":
        keyboard.send("play/pause")

    # Respond with HTTP 200 OK
    return {"status": "success", "message": "Command executed and logged successfully."}


@app.get("/logs")
def view_logs():
    """
    Endpoint for debugging. Returns the entire command history as a JSON object.
    """
    cursor.execute("SELECT * FROM access_logs ORDER BY timestamp DESC")
    records = cursor.fetchall()

    return {
        "total_records": len(records),
        "history": records
    }