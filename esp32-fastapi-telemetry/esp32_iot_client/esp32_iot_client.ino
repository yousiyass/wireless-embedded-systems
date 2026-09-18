#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

const char *send_temp = "http://YOUR_PC_IP:8000/machine_update";

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int time_interval = 3000;
double present_continuous = 0.0;

#define POT 33

int machine_sequence = 1;
int temp_data;

// EMPTY BOX (frame only)
byte emptyBox[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111
};

// Completely full, complete box
byte fullBox[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

// The fan level from Python (0, 1, 2, or 3) is passed here.
void draw_fan_graphic(int fanLevel) {
  // Move the cursor to the line below, next to the word "fan" (for example, column 9, row 1).
  lcd.setCursor(9, 1); 
  
  // A loop that draws three boxes side by side.
  for(int i = 1; i <= 3; i++) {
    if(fanLevel >= i) {
      lcd.write(1); // Press the FULL BOX in memory number 1.
    } 
    else {
      lcd.write(0); // Press the EMPTY BOX in memory number 0.
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, emptyBox);
  lcd.createChar(1, fullBox);

  pinMode(POT, INPUT);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to the network.");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (millis() - present_continuous >= time_interval) {
      present_continuous = millis();
      if (machine_sequence == 4) {
        machine_sequence = 1;
      }
      HTTPClient http;
      http.begin(send_temp);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      char send_temp_data[50];
      if (machine_sequence == 1) {
        int pot_data = analogRead(POT);
        temp_data = map(pot_data, 0, 4095, 20, 96);
      }
      else if (machine_sequence == 2) {
        temp_data = random(45, 70);
      }
      else {
        temp_data = random(70, 96);
      }
      snprintf(send_temp_data, sizeof(send_temp_data), "temp=%d&machine_id=%d", temp_data, machine_sequence);
      int reply_code = http.POST(send_temp_data);
      if (reply_code > 0) {
        if (reply_code == 200) {
          Serial.println("The package has been sent.");

          String incoming_fan_speed_string = http.getString();
          int incoming_fan_speed = incoming_fan_speed_string.toInt();

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Machine");
          lcd.print(machine_sequence);
          draw_fan_graphic(incoming_fan_speed);
          
        }
        else {
          Serial.print("The package has been sent, but there is a problem.");
          Serial.println(reply_code);
        }
      }
      else {
        Serial.println("The package could not be sent.");
      }
      http.end();
      machine_sequence += 1;
    }
  }
  else {
    Serial.println("Network connection lost; retrying.");
  }
}
