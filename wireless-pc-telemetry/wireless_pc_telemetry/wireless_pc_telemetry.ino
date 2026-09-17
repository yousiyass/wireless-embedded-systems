#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

// --- LCD Display Setup ---
// 0x27 is the standard I2C address for most 16x2 LCD modules
LiquidCrystal_I2C lcd(0x27, 16, 2);  

// --- Network Configuration ---
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

// Target FastAPI server endpoint for telemetry data
const char *server_endpoint = "http://YOUR_PC_IP:8000/cpu_gpu_telemetry";

// --- Timing Variables ---
unsigned long last_fetch_time = 0;
const int FETCH_INTERVAL = 5000; // Fetch data every 5 seconds

void setup() {
  // Initialize LCD and Serial monitor
  lcd.init();   
  lcd.backlight();
  lcd.clear();

  Serial.begin(115200);
  
  // --- Network Connection Routine ---
  WiFi.begin(ssid, password);       
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nNetwork Connected!");
}

void loop() {
  // Non-blocking delay for periodic data fetching
  if (millis() - last_fetch_time >= FETCH_INTERVAL) {
    last_fetch_time = millis();
    lcd.clear();

    // Ensure WiFi connection is still active before making HTTP requests
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(server_endpoint);
      
      int http_response_code = http.GET();
      
      if (http_response_code > 0) {
        Serial.println("Success: Data received from server.");
        
        if (http_response_code == 200) {
          String payload = http.getString();

          // --- JSON Parsing ---
          // Allocate memory pool for JSON document
          StaticJsonDocument<200> doc;
          DeserializationError error = deserializeJson(doc, payload);

          if (!error) {
            // Extract telemetry metrics
            float cpu_usage = doc["cpu"];
            float gpu_temp = doc["gpu"];

            // --- UI Update (LCD) ---
            lcd.setCursor(0, 0); 
            lcd.print("CPU: ");
            lcd.print(cpu_usage);
            lcd.print("%");

            lcd.setCursor(0, 1);
            lcd.print("GPU: ");
            lcd.print(gpu_temp);
            lcd.print("C"); // Corrected from '%' to 'C' since it's temperature
          } else {
            Serial.println("Error: JSON parsing failed.");
          }
        }
      }
      else {
        Serial.print("Error: HTTP GET failed. Code: ");
        Serial.println(http_response_code);
      }
      http.end(); // Free resources
    }
    else {
      Serial.println("Warning: WiFi Disconnected!");
      lcd.setCursor(0, 0);
      lcd.print("WiFi Disconnected");
    }
  }
}








