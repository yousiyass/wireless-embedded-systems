#include <IRremote.hpp>
#include <WiFi.h>
#include <HTTPClient.h>

// --- Network Configuration ---
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

// Target FastAPI server endpoint (Ensure PC and ESP32 are on the same network)
const char *server_endpoint = "http://YOUR_PC_IP:8000/cpu_gpu_telemetry";

// --- Pin Definitions ---
#define IR_RECEIVER_PIN 19

#define LED_NEXT_TRACK 21
#define LED_PREV_TRACK 22
#define LED_VOL_UP 23
#define LED_VOL_DOWN 25
#define LED_PLAY_PAUSE 26

// --- Timing Variables ---
unsigned long last_ir_read = 0;
const int IR_READ_INTERVAL = 1500; // Prevents IR signal flooding to the server

void setup() {
  Serial.begin(115200);
  
  // Initialize IR Receiver with built-in LED feedback
  IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);

  pinMode(LED_NEXT_TRACK, OUTPUT);
  pinMode(LED_PREV_TRACK, OUTPUT);
  pinMode(LED_VOL_UP, OUTPUT);
  pinMode(LED_VOL_DOWN, OUTPUT);
  pinMode(LED_PLAY_PAUSE, OUTPUT);

  // --- Network Connection Routine ---
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  // Blocking loop until WiFi is connected (Crucial for HTTP POST requests)
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nNetwork Connected!");
}

void loop() {
  // Non-blocking interval check to prevent spamming the FastAPI server
  if (millis() - last_ir_read >= IR_READ_INTERVAL) {
    last_ir_read = millis();

    if (IrReceiver.decode()) {
      int ir_code = IrReceiver.decodedIRData.command;
      String command_str = map_ir_to_command(ir_code);

      // Only proceed if a valid media command was mapped
      if (command_str != "unknown") {
        char post_payload[100];
        snprintf(post_payload, sizeof(post_payload), "command=%s", command_str.c_str());

        // --- HTTP POST Routine ---
        HTTPClient http;
        http.begin(server_endpoint);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Standard form data header
        
        int http_response_code = http.POST(post_payload);
        
        if (http_response_code == 200) {
          Serial.println("Success: Server executed the command.");
          trigger_visual_feedback(ir_code);
        } else {
          Serial.print("Error: HTTP POST failed. Code: ");
          Serial.println(http_response_code);
        }
        http.end(); // Free resources
      }
      IrReceiver.resume(); // Ready for the next signal
    }
  }
}

// Maps raw IR HEX codes to semantic string commands
String map_ir_to_command(int raw_code) {
  if (raw_code == 90) return "next_track";
  if (raw_code == 8)  return "prev_track";
  if (raw_code == 24) return "vol_up";
  if (raw_code == 82) return "vol_down";
  if (raw_code == 28) return "play_pause";
  if (raw_code == 22) return "mute";
  return "unknown";
}

// Triggers the specific status LED for 1 second based on the executed command
void trigger_visual_feedback(int raw_code) {
  int target_pin = -1;
  
  if (raw_code == 24) target_pin = LED_VOL_UP;
  else if (raw_code == 82) target_pin = LED_VOL_DOWN;
  else if (raw_code == 90) target_pin = LED_NEXT_TRACK;
  else if (raw_code == 8)  target_pin = LED_PREV_TRACK;
  else if (raw_code == 28) target_pin = LED_PLAY_PAUSE;

  if (target_pin != -1) {
    digitalWrite(target_pin, HIGH);
    delay(1000); // Replaced empty while-loop with safe delay to prevent WDT panic
    digitalWrite(target_pin, LOW);
  }
}