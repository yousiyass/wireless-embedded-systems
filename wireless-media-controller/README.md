# ESP32 Wireless Media & Telemetry Controller

https://github.com/user-attachments/assets/cd85dc71-f89e-4fba-9879-b017099fb9d5

A complete IoT integration bridging physical hardware and PC media management. This project captures standard infrared (IR) remote signals via an ESP32 and transmits them over a local Wi-Fi network as HTTP POST requests to a custom FastAPI server. The backend executes PC media actions (volume, track control) while persistently logging all events into an SQLite database.

> **Project Evolution & Scale-Up:** 
> This system is an advanced, wireless IoT upgrade of my previous Arduino-based media controller. By migrating from a local, wired UART architecture to an ESP32 Wi-Fi module communicating with a Python web server, this project demonstrates a complete transition from basic microcontroller operations to modern edge-to-server network integration.

## System Architecture

*   **Edge Device (ESP32):** Decodes IR signals and handles wireless HTTP communication. Provides visual confirmation via a dedicated LED array.
*   **Backend Server (FastAPI):** Asynchronous Python web server that listens for incoming HTTP requests on your local network.
*   **Execution & Auditing:** Uses the `keyboard` library for system-level media control and `sqlite3` for local event logging.

## Hardware Configuration

| Component | ESP32 GPIO | Description |
| :--- | :--- | :--- |
| **IR Receiver** | 19 | Standard 38kHz IR receiver |
| **LED 1** | 21 | Visual feedback for "Next Track" |
| **LED 2** | 22 | Visual feedback for "Previous Track" |
| **LED 3** | 23 | Visual feedback for "Volume Up" |
| **LED 4** | 25 | Visual feedback for "Volume Down" |
| **LED 5** | 26 | Visual feedback for "Play/Pause" |

## Software Stack
*   **C++ / Arduino IDE:** `IRremote.hpp`, `WiFi.h`, `HTTPClient.h`
*   **Python 3:** `fastapi`, `uvicorn`, `sqlite3`, `keyboard`

## Installation & Usage

### 1. Backend Setup (PC)
1. Navigate to the `fastapi_server` directory.
2. Install the required libraries: 
   ```bash
   pip install fastapi uvicorn keyboard
