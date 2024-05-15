#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include "time.h"

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);
Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

const char* ssid     = "notyourwifi";
const char* password = "Santa77Cruz";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

// OLED FeatherWing buttons map to different pins depending on board:
// 32u4, M0, M4, nrf52840, esp32-s2 and 328p
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5

int page;
int resync;
int sel;







void setup() {
  Serial.begin(115200);
  delay(500);

  /////////////////////// Display Init /////////////////////// 
  Serial.println("OLED init...");
  display.begin(0x3C, true); // Address 0x3C default
  display.setRotation(3);
  display.display();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  delay(500);

  page = 1;
  resync = 0;
  sel = 0;

  // Show image buffer on the display hardware
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen

  // Clear the buffer
  display.clearDisplay();
  display.display();


  /////////////////////// Buttons Init ///////////////////////
  // Initialize buttons
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  // Assign Interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_C), pageUp, FALLING);     // Page up
  attachInterrupt(digitalPinToInterrupt(BUTTON_B), select, FALLING);   // Select
  attachInterrupt(digitalPinToInterrupt(BUTTON_A), pageDown, FALLING);   // Page down

  /////////////////////// WiFi Init ///////////////////////
  Serial.println("Initializing WiFi");
  display.setCursor(0,0);
  display.print("WiFi init");
  display.display();
  WiFi.begin(ssid, password);
  delay(300);
  for (int i = 0; i < 5; i++)
  {
    delay(300);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  Serial.println("");
  Serial.println("Connected");
  display.println("");
  display.println("Connected");
  display.display();
  delay(500);

  /////////////////////// Temp Sensor Init ///////////////////////
  Serial.println("Initializing BME280...");
  display.println("BME280 init...");
  
  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();

  /////////////////////// Time Init ///////////////////////
  Serial.print("Fetching time...");
  display.print("Fetching time...");
  display.display();
  for (int i = 0; i < 5; i++)
  {
    delay(300);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  delay(500);

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}







void loop() {
  delay(1000);  // Display refresh rate
  resync++;
  if (resync > 86399){
    // Recalibrate after 24h
  }

  //Serial.println(page);

  switch (page) {
    case 0: 
      Serial.println("Infomation");
      Serial.print("Select = "); Serial.println(sel);

      display.clearDisplay();
      display.display();
      display.setCursor(0,0);
      display.setTextSize(2);
      display.println("Info");
      display.setCursor(0,20);
      display.setTextSize(1);
      //getBattInfo(battLevel);
      display.print("Battery Level: "); display.println("xx %");
      //readBME280();
      display.print("Temperature: "); display.print("xx "); display.print((char)247); display.println("C"); // Char247 = degree symbol
      display.print("Humidity: "); display.print("xx "); display.println("%");
      display.print("Pressure: "); display.print("xx "); display.println("hPa");
      display.display();
    break;

    case 1: 
      Serial.println("Time");
      Serial.print("Select = "); Serial.println(sel);
      
      if (sel == 1) {
        display.clearDisplay();
        display.display();
      }
      else {
        display.clearDisplay();
        display.display();
        yield();
        printLocalTime();
        display.display();
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      }
    break;

    case 2: 
      Serial.println("Weather");
      Serial.print("Select = "); Serial.println(sel);

      display.clearDisplay();
      display.display();
      display.setCursor(0,0);
      display.setTextSize(2);
      display.println("Weather");
      display.display();
      break;
  }

}







void printLocalTime() {
  struct tm timeinfo;

  // Error trap
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    display.print("Failed to obtain time");
    display.display();
    delay(2000);
    return;
  }
  else
  {
  // Clear the display
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  display.setTextSize(2);

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  display.println(&timeinfo, "%A \n%B %d \n%H:%M:%S");
  
  /*Examples:
  Serial.print("Day of week: "); Serial.println(&timeinfo, "%A");
  Serial.print("Month: "); Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: "); Serial.println(&timeinfo, "%d");
  Serial.print("Year: "); Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: "); Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): "); Serial.println(&timeinfo, "%I");
  Serial.print("Minute: "); Serial.println(&timeinfo, "%M");
  Serial.print("Second: "); Serial.println(&timeinfo, "%S");
  display.display();

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  display.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();  
  display.println(timeWeekDay);
  display.println();
  display.display(); */
  }
}







void pageUp() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  sel = 0;
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    page++;
    if (page > 2){
      page = 0;
      sel = 0;
    }
    Serial.print("Page up: "); Serial.println(page);
  }
  last_interrupt_time = interrupt_time;
}






void pageDown() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  sel = 0;
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    page--;
    if (page < 0){
      page = 2;
    }
    Serial.print("Page down: ");
    Serial.println(page);
  }
  last_interrupt_time = interrupt_time;
}






void select() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    if (sel == 1) {
      sel = 0;
    }
    else {
      sel = 1;
    }
  }
  // Serial.println("Select = 1");
  last_interrupt_time = interrupt_time;
}







void readBME280() {
  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);
}