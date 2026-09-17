#include <WiFi.h>
#include <HTTPClient.h>

// --- Network Configuration ---
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

// --- API Endpoints ---
// Binance Futures public API endpoints for real-time ticker prices
const char *btc_price_url = "https://fapi.binance.com/fapi/v1/ticker/price?symbol=BTCUSDT";
const char *snx_price_url = "https://fapi.binance.com/fapi/v1/ticker/price?symbol=SNXUSDT";

// Target local FastAPI server endpoint for forwarding data
const char *server_endpoint = "http://YOUR_PC_IP:8000/receive_data";

// --- Timing Variables ---
unsigned long last_process_time = 0;
const unsigned long PROCESS_INTERVAL = 5000; // Fetch and send every 5 seconds

unsigned long dot_timer = 0;
int dot_interval = 500;

String btc_payload;
String snx_payload;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // --- Network Connection Routine ---
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - dot_timer >= dot_interval) {
      dot_timer = millis();
      Serial.print(".");
    }
  }
  Serial.println("\nNetwork Connected!");
}

void loop() {
  // Non-blocking interval check for fetching and posting data
  if (millis() - last_process_time >= PROCESS_INTERVAL) {
    last_process_time = millis();

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http_client;

      // --- Fetch Bitcoin Price ---
      http_client.begin(btc_price_url);
      int btc_response_code = http_client.GET();
      if (btc_response_code > 0) {
        btc_payload = http_client.getString();
      }
      http_client.end();

      // --- Fetch Altcoin (SNX) Price ---
      http_client.begin(snx_price_url);
      int snx_response_code = http_client.GET();
      if (snx_response_code > 0) {
        snx_payload = http_client.getString();
      }
      http_client.end();

      char post_payload[100];

      // --- String Parsing for JSON Ticker Payload ---
      // Example Binance JSON response: {"symbol":"BTCUSDT","price":"65000.50","time":...}
      int btc_idx = btc_payload.indexOf("price");
      if (btc_idx != -1) {
        int btc_start = btc_idx + 8;
        int btc_end = btc_payload.indexOf("\"", btc_start);
        String btc_val_str = btc_payload.substring(btc_start, btc_end);
        float btc_float = btc_val_str.toFloat();

        int snx_idx = snx_payload.indexOf("price");
        if (snx_idx != -1) {
          int snx_start = snx_idx + 8;
          int snx_end = snx_payload.indexOf("\"", snx_start);
          String snx_val_str = snx_payload.substring(snx_start, snx_end);
          float snx_float = snx_val_str.toFloat();

          // Format data into URL-encoded form data standard
          snprintf(post_payload, sizeof(post_payload), "BTCUSDT=%.2f&SNXUSDT=%.3f", btc_float, snx_float);

          // --- Send Data to Local FastAPI Server via HTTP POST ---
          HTTPClient post_client;
          post_client.begin(server_endpoint);
          post_client.addHeader("Content-Type", "application/x-www-form-urlencoded");
          
          int server_response = post_client.POST(post_payload);
          Serial.println(post_payload);

          if (server_response > 0) {
            Serial.println("Success: Data transmitted to server.");
            if (server_response == 200) {
              Serial.println("Status 200: Server processed successfully.");
            }
          } else {
            Serial.print("Error: POST request failed. Code: ");
            Serial.println(server_response);
          }
          post_client.end();
        }
      }
    } else {
      Serial.println("Warning: WiFi Connection Lost! Reconnecting...");
    }
  }
}