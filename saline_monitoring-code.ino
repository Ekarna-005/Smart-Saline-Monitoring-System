#include <WiFi.h>
#include <HTTPClient.h>
#include <HX711.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DOUT 4
#define CLK 5

HX711 scale;

#define GREEN_LED 18
#define YELLOW_LED 19
#define RED_LED 23

#define BUZZER 15

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

String apiKey = "YOUR_API_KEY";

float bottleWeight = 500.0;
float lowLevel = 100.0;

void setup() {

  Serial.begin(115200);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  display.println("WiFi Connected");
  display.display();
  delay(1000);
}

void loop() {

  float weight = scale.get_units(10);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(weight);
  display.println(" g");

  float remaining = weight - bottleWeight;

  display.setTextSize(1);
  display.print("Remaining:");
  display.print(remaining);
  display.println(" g");

  if (remaining > 250) {

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);

    display.println("Status : Normal");

  } else if (remaining > lowLevel) {

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);

    display.println("Status : Low");

  } else {

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    tone(BUZZER, 1000);

    display.println("Replace Bottle!");
  }

  display.display();

  sendData(weight);

  delay(5000);
}

void sendData(float weight) {

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    String url =
      "http://api.thingspeak.com/update?api_key=" + apiKey +
      "&field1=" + String(weight);

    http.begin(url);

    http.GET();

    http.end();
  }
}