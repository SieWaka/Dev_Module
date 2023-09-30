// LLamado de librerias a utilizar
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Configuracion Identificador  modulo
int deviceID = 1;

//Configuracion ubicacion modulo
float latitude = -34.62994536;
float longitude = -58.39187918;

//Configuración Sensor PH
#define PH_PIN 33
float acidVoltage    = 1876;    //buffer solution 4.0 at 25C
float neutralVoltage = 1414;     //buffer solution 7.0 at 25C
float slopePH, interceptPH, slopeEC, interceptEC = 1;

//Configuración sensor EC
#define EC_PIN 32
#define RES2 820.0
#define ECREF 50
float kValue = 0.993;
float highec    = 1390;    //buffer solution 4.0 at 25C
float lowec = 60;     //buffer solution 7.0 at 25C

//Configuración sensor temperatura
OneWire ourWire(15);                //Se establece el pin 4  como bus OneWire para el sensor de temperatura
DallasTemperature sensors(&ourWire); //Se declara una variable u objeto para nuestro sensor

//Configuración variables
bool ok_read = false;
float  voltagePH, voltageEC, phValue, ecValue, temperature = 19;

// Red, green, and blue pins for PWM control
const int redPin = 13;     // 13 corresponds to GPIO13
const int greenPin = 12;   // 12 corresponds to GPIO12
const int bluePin = 14;    // 14 corresponds to GPIO14

// Setting PWM frequency, channels and bit resolution
const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;

// Bit resolution 2^8 = 256
const int resolution = 8;

//Configuración pantalla
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define down_pin 36
#define up_pin 4
#define enter_pin 34
#define back_pin 35
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// The MQTT topics, cargar información y recibir información
#define AWS_IOT_PUBLISH_TOPIC   "samples/upload"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net;
//MQTTClient client = MQTTClient(256);
PubSubClient client(net);

static const uint8_t image_data_Siewaka[1024] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xf0, 0x3e, 0x7f, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0x0c, 0x3f, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xfe, 0x09, 0xff, 0xff, 0x80, 0x06, 0x7f, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xfc, 0x0c, 0xff, 0xff, 0x8f, 0x87, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xf8, 0x0e, 0xff, 0xff, 0x0f, 0xef, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xf8, 0x0f, 0x7f, 0xff, 0x1f, 0xfe, 0x78, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xf0, 0x0f, 0x3f, 0xff, 0x8f, 0xfc, 0x70, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xe0, 0x0f, 0x9f, 0xff, 0x81, 0xfc, 0x71, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xc0, 0x0e, 0x0f, 0xff, 0xc0, 0x3c, 0x63, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xc6, 0x08, 0x4f, 0xff, 0xe0, 0x0c, 0x63, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x8e, 0x01, 0xe7, 0xff, 0xfe, 0x04, 0x60, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x0c, 0x07, 0xf7, 0xff, 0xff, 0x84, 0x60, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x08, 0x0f, 0xf3, 0xff, 0xff, 0xc4, 0x63, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xfe, 0x38, 0x0f, 0xfb, 0xff, 0xbf, 0xc4, 0x63, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xfe, 0x30, 0x0f, 0xf9, 0xff, 0x0f, 0x84, 0x61, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xfc, 0x70, 0x0f, 0xf9, 0xff, 0x00, 0x0c, 0x70, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xfc, 0x60, 0x0f, 0xe0, 0xff, 0x80, 0x1c, 0x78, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xfc, 0x60, 0x0f, 0x80, 0xff, 0xe0, 0x3e, 0x7e, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xfc, 0xe0, 0x0c, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0xc0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0xc0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0xc0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf9, 0xc0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf9, 0xc0, 0x00, 0x00, 0x7f, 0x9f, 0xff, 0xf1, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0x80, 0x00, 0x00, 0x7f, 0x0f, 0xff, 0xf1, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0x00, 0x00, 0x00, 0x7f, 0x0f, 0xff, 0xe1, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0x00, 0x00, 0x00, 0x7f, 0x8f, 0xff, 0xe3, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0x00, 0x00, 0x00, 0x7f, 0x8f, 0xc7, 0xe3, 0xe0, 0xce, 0x3e, 0x7c, 0x19, 0xff, 0xff,
  0xff, 0xf8, 0x00, 0x00, 0x00, 0xff, 0x87, 0xc3, 0xc3, 0x80, 0x0e, 0x3c, 0x78, 0x00, 0xff, 0xff,
  0xff, 0xfc, 0x00, 0x00, 0x00, 0xff, 0xc7, 0x83, 0xc7, 0x00, 0x0e, 0x38, 0x70, 0x00, 0xff, 0xff,
  0xff, 0xfc, 0x00, 0x00, 0x00, 0xff, 0xc7, 0x83, 0xc7, 0x1f, 0x0e, 0x30, 0xe1, 0xe0, 0xff, 0xff,
  0xff, 0xfc, 0x00, 0x00, 0x01, 0xff, 0xc3, 0x81, 0x86, 0x1f, 0x8e, 0x21, 0xe3, 0xf0, 0xff, 0xff,
  0xff, 0xfe, 0x00, 0x00, 0x01, 0xff, 0xe3, 0x11, 0x8e, 0x3f, 0x8e, 0x03, 0xe3, 0xf8, 0xff, 0xff,
  0xff, 0xff, 0x00, 0x00, 0x03, 0xff, 0xe3, 0x18, 0x8e, 0x3f, 0x8e, 0x07, 0xe3, 0xf8, 0xff, 0xff,
  0xff, 0xff, 0x00, 0x00, 0x03, 0xff, 0xe0, 0x38, 0x0e, 0x3f, 0x8e, 0x03, 0xe3, 0xf8, 0xff, 0xff,
  0xff, 0xff, 0x80, 0x00, 0x07, 0xff, 0xf0, 0x38, 0x1e, 0x3f, 0x8e, 0x01, 0xe3, 0xf8, 0xff, 0xff,
  0xff, 0xff, 0xc0, 0x00, 0x0f, 0xff, 0xf0, 0x7c, 0x1e, 0x1f, 0x8e, 0x30, 0xe3, 0xf0, 0xff, 0xff,
  0xff, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0xf8, 0x7c, 0x1f, 0x0e, 0x0e, 0x38, 0x71, 0xe0, 0xff, 0xff,
  0xff, 0xff, 0xf0, 0x00, 0x7f, 0xff, 0xf8, 0x7e, 0x3f, 0x80, 0x0e, 0x3c, 0x30, 0x00, 0xff, 0xff,
  0xff, 0xff, 0xfc, 0x00, 0xff, 0xff, 0xf8, 0xfe, 0x3f, 0xc0, 0x0e, 0x7e, 0x38, 0x08, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xfd, 0xff, 0x7f, 0xe1, 0xdf, 0xff, 0xfe, 0x1d, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};


int selected = 0;
int entered = -1;

void connectAWS() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("MODULO SIE WAKA"));
  display.println("Modo ON LINE");
  ledcWrite(redChannel, 10);
  ledcWrite(greenChannel, 255);
  ledcWrite(blueChannel, 255);
  delay(2000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  display.println("Connecting to Wi-Fi");
  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  ledcWrite(redChannel, 255);
  ledcWrite(greenChannel, 255);
  ledcWrite(blueChannel, 10);
  delay(2000);

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(callback);

  display.print("Connecting to AWS IOT");
  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    ledcWrite(redChannel, 10);
    ledcWrite(greenChannel, 255);
    ledcWrite(blueChannel, 255);
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  display.println("AWS IoT Connected!");
  ledcWrite(redChannel, 255);
  ledcWrite(greenChannel, 10);
  ledcWrite(blueChannel, 255);
  delay(2000);
}

void publishMessage(float deviceID, float latitude, float longitude, float temperature, float ecValue, float phValue) {

  StaticJsonDocument<256> doc;
  JsonObject info = doc.to<JsonObject>();
  info["deviceId"] = deviceID;
  info["latitude"] = latitude;
  info["longitude"] = longitude;
  JsonObject measurementValues = info.createNestedObject("measurementValues");
  measurementValues["Temperatura del Agua [°Celsius]"] = temperature;
  measurementValues["Conductividad [µs/cm]"] = ecValue;
  measurementValues["pH [Unidades de pH]"] = phValue;
  info["takenAt"] = "2023-08-24T13:10:10Z";
  info["apiKey"] = "589be0f62f4502b8490eccaf748cf194";

  char jsonBuffer[512];

  serializeJson(info, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

}

void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = " ";
  for (int i = 0; i < length; i++) incommingMessage += (char)payload[i];
  Serial.println("Message arrived [" + String(topic) + "] " + incommingMessage);
  // check for other commands
  /* else if( strcmp(topic,command2_topic) == 0){
    if (incommingMessage.equals(“1”)) { } // do something else
    }
  */

}
//void loopmeasure() {
//  client.loop();
//  static unsigned long timepoint = millis();
//  if (millis() - timepoint > 6000U) {
//    //time interval: 1s
//    timepoint = millis();
//    sensors.requestTemperatures();   //Se envía el comando para leer la temperatura
//    temperature = sensors.getTempCByIndex(0);
//    Serial.print("Temperatura: ");
//    Serial.print(temperature, 2);
//    Serial.println(" ºC");
//    voltagePH = analogRead(PH_PIN) / 4095.0 * 3300;      // read the ph voltage
//    slopePH = (7.0 - 4.0) / ((neutralVoltage - 1500) / 3.0 - (acidVoltage - 1500) / 3.0); // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
//    interceptPH =  7.0 - slopePH * (neutralVoltage - 1500) / 3.0;
//    phValue = slopePH * (voltagePH - 1500) / 3.0 + interceptPH; //y = k*x + b
//    Serial.print("pH: ");
//    Serial.println(phValue, 2);
//    voltageEC = analogRead(EC_PIN) / 4095.0 * 3300;
//    slopeEC = (12.88 - 1.41) / ((highec) - (lowec));
//    interceptEC =  (12.88 - slopeEC * highec);
//    ecValue = slopeEC * voltageEC + interceptPH - 6.44 ; //y = k*x + b
//    Serial.print("EC: ");
//    Serial.print(ecValue, 2);
//    Serial.println(" ms/cm");
//    ok_read = true;
//  }
//  if (ok_read == true) {
//    ledcWrite(redChannel, 255);
//    ledcWrite(greenChannel, 255);
//    ledcWrite(blueChannel, 255);
//    publishMessage(float(deviceID), float(latitude), float(longitude), float(temperature), float(ecValue), float(phValue));
//    ok_read = false;
//    delay(500);
//    ledcWrite(redChannel, 255);
//    ledcWrite(greenChannel, 10);
//    ledcWrite(blueChannel, 255);
//  }
//}


void displaymenu(void) {

  int down = analogRead(down_pin);
  int up = analogRead(up_pin);
  int enter = analogRead(enter_pin);
  int back = analogRead(back_pin);

  if (up == 0 && down == 0) {
  };
  if (up == 0) {
    selected = selected + 1;
    if (selected > 3) {
      selected = 3;
    }
    delay(200);
  };
  if (down == 0) {
    selected = selected - 1;
    if (selected < 0) {
      selected = 0;
    }
    delay(200);
  };
  if (enter == 0) {
    entered = selected;
  };
  if (back == 0) {
    entered = -1;
  };

  Serial.print(selected);
  Serial.print('\t');
  Serial.println(entered);


  const char *options[4] = {
    " Medicion OFF LINE ",
    " Medicion ON LINE ",
    " Calibrar ",
    " Info Device ",
  };

  if (entered == -1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Menu Principal"));
    display.println("");
    for (int i = 0; i < 4; i++) {
      if (i == selected) {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println(options[i]);
      } else if (i != selected) {
        display.setTextColor(SSD1306_WHITE);
        display.println(options[i]);
      }
    }
  } else if (entered == 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("MODULO SIE WAKA"));
    display.println("Modo OFF LINE");
    sensors.requestTemperatures();   //Se envía el comando para leer la temperatura
    temperature = sensors.getTempCByIndex(0);
    display.setTextColor(SSD1306_WHITE);
    display.print("Temp: ");
    display.print(temperature);
    display.println(" ºC");
    voltagePH = analogRead(PH_PIN) / 4095.0 * 3300;      // read the ph voltage
    slopePH = (7.0 - 4.0) / ((neutralVoltage - 1500) / 3.0 - (acidVoltage - 1500) / 3.0); // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
    interceptPH =  7.0 - slopePH * (neutralVoltage - 1500) / 3.0;
    phValue = slopePH * (voltagePH - 1500) / 3.0 + interceptPH; //y = k*x + b
    display.print("pH: ");
    display.println(phValue);
    voltageEC = analogRead(EC_PIN) / 4095.0 * 3300;
    slopeEC = (12.88 - 1.41) / ((highec) - (lowec));
    interceptEC =  (12.88 - slopeEC * highec);
    ecValue = slopeEC * voltageEC + interceptPH - 6.44 ; //y = k*x + b
    display.print("EC: ");
    display.print(ecValue);
    display.println(" ms/cm");
    delay(1000);

  } else if (entered == 1) {
    connectAWS();
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("MODULO SIE WAKA"));
    display.println("Modo ON LINE");
    display.setTextColor(SSD1306_WHITE);
    client.loop();
    sensors.requestTemperatures();   //Se envía el comando para leer la temperatura
    temperature = sensors.getTempCByIndex(0);
    display.print("Temp: ");
    display.print(temperature);
    display.println(" ºC");
    voltagePH = analogRead(PH_PIN) / 4095.0 * 3300;      // read the ph voltage
    slopePH = (7.0 - 4.0) / ((neutralVoltage - 1500) / 3.0 - (acidVoltage - 1500) / 3.0); // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
    interceptPH =  7.0 - slopePH * (neutralVoltage - 1500) / 3.0;
    phValue = slopePH * (voltagePH - 1500) / 3.0 + interceptPH; //y = k*x + b
    display.print("pH: ");
    display.println(phValue);
    voltageEC = analogRead(EC_PIN) / 4095.0 * 3300;
    slopeEC = (12.88 - 1.41) / ((highec) - (lowec));
    interceptEC =  (12.88 - slopeEC * highec);
    ecValue = slopeEC * voltageEC + interceptPH - 6.44 ; //y = k*x + b
    display.print("EC: ");
    display.print(ecValue);
    delay(4000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("MODULO SIE WAKA"));
    display.println("Modo ON LINE");
    display.println("Lectura Tomada");
    delay(2000);
    ok_read = true;
    if (ok_read == true) {
      ledcWrite(redChannel, 255);
      ledcWrite(greenChannel, 255);
      ledcWrite(blueChannel, 255);
      publishMessage(float(deviceID), float(latitude), float(longitude), float(temperature), float(ecValue), float(phValue));
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println(F("MODULO SIE WAKA"));
      display.println("Modo ON LINE");
      display.println("Lectura Enviada");
      delay(2000);
      ledcWrite(redChannel, 255);
      ledcWrite(greenChannel, 10);
      ledcWrite(blueChannel, 255);
      ok_read = false;
      entered = -1;
    }
  }
  else if (entered == 2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("MODULO SIE WAKA"));
    display.println("Calibracion");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println("Calibration");
  } else if (entered == 3) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("MODULO SIE WAKA"));
    display.println("Info. del Sistema");
    display.print("Device ID: ");// demas info
    display.println(deviceID);
    display.print("Latitud: ");// demas info
    display.println(latitude);
    display.print("Longitud: ");// demas info
    display.println(longitude);
  }

  display.display();
}
void setup() {
  Serial.begin(115200);

  pinMode(down_pin, INPUT);
  pinMode(up_pin, INPUT);
  pinMode(enter_pin, INPUT);
  pinMode(back_pin, INPUT);

  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  sensors.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000); // Pause for 2 seconds
  ledcWrite(redChannel, 255);
  ledcWrite(greenChannel, 255);
  ledcWrite(blueChannel, 255);

  // Clear the buffer.
  display.clearDisplay();

  // Draw bitmap on the screen
  display.drawBitmap(0, 0, image_data_Siewaka, 128, 64, 1);
  display.display();
  delay(2000);
}

void loop() {
  displaymenu();
}