# wireless-embedded-systems
ESP32 and IoT projects featuring wireless communication, web servers, and HTML dashboards.

# Embedded Systems & IoT Portfolio 🚀

A collection of embedded systems and IoT projects bridging physical microcontrollers (`ESP32` / `Arduino`) with local Python (`FastAPI`) backends, SQLite databases, and wireless network communications.

> **Project Evolution:** 
> This portfolio represents a technical progression from basic, wired Arduino circuits to modern, wireless edge-to-server architectures utilizing ESP32 Wi-Fi modules, REST APIs, and automated database logging.

---

## 📂 Projects

1. **Wireless Media & Telemetry Controller**  
   An advanced, wireless evolution of my previous Arduino media controller. Decodes IR remote signals on an ESP32 and sends HTTP requests over Wi-Fi to a Python server for PC media control and logging.

2. **PC Hardware Telemetry System**  
   A wireless upgrade from my earlier serial telemetry projects. An ESP32 fetches real-time CPU utilization and GPU temperature from a Python backend and displays live metrics on an I2C LCD screen.

3. **Binance Crypto Tracker**  
   Connects an ESP32 directly to external internet APIs (Binance Futures) to track live cryptocurrency prices over Wi-Fi, persisting the financial data into a local SQLite database.

4. **Web Dashboard**  
   A custom web interface designed to visualize and interact with the collected system data in real-time.

---

## ⚙️ Quick Setup

1. **Backend Setup (Python):**
   ```bash
   pip install fastapi uvicorn psutil GPUtil
   uvicorn server_name:app --host 0.0.0.0 --port 8000 --reload
