#define BLYNK_TEMPLATE_ID "TMPLLcifPANT"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "IbMxlb0PTFrKRF1qPX5aYrbGEfaqF8l6"

// Your WiFi credentials.
char ssid[] = "chobang 2.4";
char pass[] = "966974189";
char auth[] = BLYNK_AUTH_TOKEN;
//ติดต่อกับ app blynk
// You should get Auth Token in the Blynk App.

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TimeLib.h>
#include <AceButton.h>
using namespace ace_button;

// define the GPIO connected with Relays and switches
#define RelayPin1 19  //19
#define RelayPin2 21  //21
#define RelayPin3 22  //22
#define RelayPin4 23  //23

#define SwitchPin1 4   //4
#define SwitchPin2 16  //16
#define SwitchPin3 17  //17
#define SwitchPin4 5   //5

#define wifiLed 0  //0 wifi

#define DHTPIN 27  //Connect Out pin to 27 in NODE MCU ESP32
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Go to the Project Settings (nut icon).
bool fetch_blynk_state = true;  //true or false
BlynkTimer timer;
long rtc_sec_server;
unsigned char weekday_server;
#define VALVE_1_OUT 12
#define VALVE_2_OUT 14
#define MOISURE_READ 33
#define DHT_READ_PIN 27

//Gas Detected แก๊ส
#define MQ2 34
#define GREEN 25
#define RED 26
#define buzzer 13
int MQ2_Val = 0;

//วาวล์น้ำ
#define VALVE_ON 0   //ปิด
#define VALVE_OFF 1  //เปิด

//ฟินจำลอง
#define BLYNK_TEMP V0
#define BLYNK_RH V1
#define BLYNK_SOIL_MOISTURE V2
#define BLYNK_TIMER_1 V3
#define BLYNK_TIMER_2 V4
#define BLYNK_VALVE_1 V5
#define BLYNK_VALVE_2 V6
#define BLYNK_EN_TIMER_1 V7
#define BLYNK_EN_TIMER_2 V8

//Change the virtual pins according the rooms
#define VPIN_BUTTON_1 V15
#define VPIN_BUTTON_2 V16
#define VPIN_BUTTON_3 V17
#define VPIN_BUTTON_4 V18
//
// Relay State
bool toggleState_1 = LOW;  //Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW;  //Define integer to remember the toggle state for relay 2
bool toggleState_3 = LOW;  //Define integer to remember the toggle state for relay 3
bool toggleState_4 = LOW;  //Define integer to remember the toggle state for relay 4

//รับค่าเซ็นเชอร์วัดเเสง
const int ledPin = 2;
const int ldrPin = 32;

int wifiFlag = 0;

//รับค่าเว็นเชอร์ตรวจจับความเคลื่อนไหว
WidgetLED led(V9);  //**********************************//
// This function is called every time the Virtual Pin 0 state changes
int PIR = 18;
int Buzzer = 13;
int PIRStatus = 0;
int Status = 0;
WidgetLCD LCD(V11);
WidgetLED LED(V12);
//

int soil_moisture;
float humidity;
float temperature;
//
ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);
ButtonConfig config4;
AceButton button4(&config4);

void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);
void handleEvent4(AceButton*, uint8_t, uint8_t);

// When App button is pushed - switch the state

BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  digitalWrite(RelayPin1, !toggleState_1);
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, !toggleState_2);
}

BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  digitalWrite(RelayPin3, !toggleState_3);
}

BLYNK_WRITE(VPIN_BUTTON_4) {
  toggleState_4 = param.asInt();
  digitalWrite(RelayPin4, !toggleState_4);
}
//
unsigned char start_time_hour_1;
unsigned char start_time_min_1;
unsigned char stop_time_hour_1;
unsigned char stop_time_min_1;
unsigned char day_timer_1;
bool flag_timer1_en;
bool flag_timer_on_valve_1;

unsigned char start_time_hour_2;
unsigned char start_time_min_2;
unsigned char stop_time_hour_2;
unsigned char stop_time_min_2;
unsigned char day_timer_2;
bool flag_timer2_en;
bool flag_timer_on_valve_2;

bool flag_valve_1_set;
bool flag_valve_2_set;

bool flag_valve_1_status;
bool flag_valve_2_status;

bool flag_blynk_valve_1_update;
bool flag_blynk_valve_2_update;

bool flag_blynk_guage_update;

bool rtc_synchronized;
//
void checkBlynkStatus() {  // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    wifiFlag = 0;
    if (!fetch_blynk_state) {
      Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
      Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
      Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
      Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
    }
    digitalWrite(wifiLed, HIGH);
    //Serial.println("Blynk Connected");
  }
}
// ######################################################################
BLYNK_CONNECTED() {
  Serial.print("BLYNK SERVER CONNECTED !!!");
  // Blynk.syncAll();
  Blynk.syncVirtual(BLYNK_TIMER_1);
  Blynk.syncVirtual(BLYNK_TIMER_2);
  Blynk.syncVirtual(BLYNK_EN_TIMER_1);
  Blynk.syncVirtual(BLYNK_EN_TIMER_2);
  Blynk.syncVirtual(BLYNK_VALVE_1);
  Blynk.syncVirtual(BLYNK_VALVE_2);
  Blynk.sendInternal("rtc", "sync");
  // Request the latest state from the server
  if (fetch_blynk_state) {
    Blynk.syncVirtual(VPIN_BUTTON_1);
    Blynk.syncVirtual(VPIN_BUTTON_2);
    Blynk.syncVirtual(VPIN_BUTTON_3);
    Blynk.syncVirtual(VPIN_BUTTON_4);
  }
}
// ######################################################################
BLYNK_WRITE(BLYNK_VALVE_1) {
  int val = param.asInt();  // assigning incomming value from pin to a var

  if (flag_timer_on_valve_1 == 0)
    flag_valve_1_set = val;
  else
    flag_blynk_valve_1_update = 1;

  Serial.print("Valve 1 Set: ");
  Serial.println(val);
}

// ######################################################################
BLYNK_WRITE(BLYNK_VALVE_2) {
  int val = param.asInt();  // assigning incomming value from pin to a var

  if (flag_timer_on_valve_2 == 0)
    flag_valve_2_set = val;
  else
    flag_blynk_valve_2_update = 1;

  Serial.print("Valve 2 Set: ");
  Serial.println(val);
}

// ######################################################################
BLYNK_WRITE(BLYNK_EN_TIMER_1) {
  int val = param.asInt();  // assigning incomming value from pin to a var
  flag_timer1_en = val;
  Serial.println("Timer 1 EN: " + String(flag_timer1_en));
}

// ######################################################################
BLYNK_WRITE(BLYNK_EN_TIMER_2) {
  int val = param.asInt();  // assigning incomming value from pin to a var
  flag_timer2_en = val;
  Serial.println("Timer 2 EN: " + String(flag_timer2_en));
}

// ######################################################################
BLYNK_WRITE(BLYNK_TIMER_1) {
  unsigned char week_day;

  TimeInputParam t(param);

  if (t.hasStartTime() && t.hasStopTime() /*&& t.getStartSecond()==0 && t.getStopSecond()==0*/) {
    start_time_hour_1 = t.getStartHour();
    start_time_min_1 = t.getStartMinute();
    Serial.println(String("Time1 Start: ") + start_time_hour_1 + ":" + start_time_min_1);

    stop_time_hour_1 = t.getStopHour();
    stop_time_min_1 = t.getStopMinute();
    Serial.println(String("Time1 Stop: ") + stop_time_hour_1 + ":" + stop_time_min_1);

    for (int i = 1; i <= 7; i++) {
      if (t.isWeekdaySelected(i))  // will be "TRUE" if nothing selected as well
      {
        day_timer_1 |= (0x01 << (i - 1));
      } else
        day_timer_1 &= (~(0x01 << (i - 1)));
    }

    Serial.print("Time1 Selected Days: ");
    Serial.println(day_timer_1, HEX);
    // flag_timer1_en = 1;
  } else {
    // flag_timer1_en = 0;
    Serial.println("Disabled Timer 1");
  }
}

// ######################################################################
BLYNK_WRITE(BLYNK_TIMER_2) {
  unsigned char week_day;

  TimeInputParam t(param);

  if (t.hasStartTime() && t.hasStopTime() /*&& t.getStartSecond()==0 && t.getStopSecond()==0*/) {
    start_time_hour_2 = t.getStartHour();
    start_time_min_2 = t.getStartMinute();
    Serial.println(String("Time2 Start: ") + start_time_hour_2 + ":" + start_time_min_2);

    stop_time_hour_2 = t.getStopHour();
    stop_time_min_2 = t.getStopMinute();
    Serial.println(String("Time2 Stop: ") + stop_time_hour_2 + ":" + stop_time_min_2);

    for (int i = 1; i <= 7; i++) {
      if (t.isWeekdaySelected(i))  // will be "TRUE" if nothing selected as well
      {
        day_timer_2 |= (0x01 << (i - 1));
      } else
        day_timer_2 &= (~(0x01 << (i - 1)));
    }

    Serial.print("Time1 Selected Days: ");
    Serial.println(day_timer_2, HEX);
    //  flag_timer2_en = 1;
  } else {
    //  flag_timer2_en = 0;
    Serial.println("Disabled Timer 2");
  }
}

// ######################################################################
BLYNK_WRITE(InternalPinRTC) {
  const unsigned long DEFAULT_TIME = 1357041600;  // Jan 1 2013
  unsigned long blynkTime = param.asLong();

  if (blynkTime >= DEFAULT_TIME) {
    setTime(blynkTime);

    weekday_server = weekday();

    if (weekday_server == 1)
      weekday_server = 7;
    else
      weekday_server -= 1;

    rtc_sec_server = (hour() * 60 * 60) + (minute() * 60) + second();

    Serial.println(blynkTime);
    Serial.println(String("RTC Server: ") + hour() + ":" + minute() + ":" + second());
    Serial.println(String("Day of Week: ") + weekday());
    rtc_synchronized = 1;
  }
}

// ######################################################################
void fn_valve_mng(void) {
  bool time_set_overflow;
  long start_timer_sec;
  long stop_timer_sec;
  bool flag_timer_on_1_buf = flag_timer_on_valve_1;
  bool flag_timer_on_2_buf = flag_timer_on_valve_2;

  // VALVE 1
  time_set_overflow = 0;
  start_timer_sec = start_time_hour_1 * 3600 + start_time_min_1 * 60;
  stop_timer_sec = stop_time_hour_1 * 3600 + stop_time_min_1 * 60;

  if (stop_timer_sec < start_timer_sec) time_set_overflow = 1;

  if (rtc_synchronized && flag_timer1_en && (((time_set_overflow == 0 && (rtc_sec_server >= start_timer_sec) && (rtc_sec_server < stop_timer_sec)) || (time_set_overflow && ((rtc_sec_server >= start_timer_sec) || (rtc_sec_server < stop_timer_sec)))) && (day_timer_1 == 0x00 || (day_timer_1 & (0x01 << (weekday_server - 1)))))) {
    flag_timer_on_valve_1 = 1;
  } else
    flag_timer_on_valve_1 = 0;

  // VALVE 2
  time_set_overflow = 0;
  start_timer_sec = start_time_hour_2 * 3600 + start_time_min_2 * 60;
  stop_timer_sec = stop_time_hour_2 * 3600 + stop_time_min_2 * 60;

  if (stop_timer_sec < start_timer_sec) time_set_overflow = 1;

  if (rtc_synchronized && flag_timer2_en && (((time_set_overflow == 0 && (rtc_sec_server >= start_timer_sec) && (rtc_sec_server < stop_timer_sec)) || (time_set_overflow && ((rtc_sec_server >= start_timer_sec) || (rtc_sec_server < stop_timer_sec)))) && (day_timer_2 == 0x00 || (day_timer_2 & (0x01 << (weekday_server - 1)))))) {
    flag_timer_on_valve_2 = 1;
  } else
    flag_timer_on_valve_2 = 0;


  // VALVE 1
  if (flag_timer_on_valve_1) {
    flag_valve_1_status = 1;
    flag_valve_1_set = 0;
  } else {
    flag_valve_1_status = flag_valve_1_set;
  }

  // VALVE 2
  if (flag_timer_on_valve_2) {
    flag_valve_2_status = 1;
    flag_valve_2_set = 0;
  } else {
    flag_valve_2_status = flag_valve_2_set;
  }

  if (flag_timer_on_1_buf != flag_timer_on_valve_1)
    flag_blynk_valve_1_update = 1;

  if (flag_timer_on_2_buf != flag_timer_on_valve_2)
    flag_blynk_valve_2_update = 1;

  // HARDWARE CONTROL
  digitalWrite(VALVE_1_OUT, !flag_valve_1_status);  // Relay active LOW
  digitalWrite(VALVE_2_OUT, !flag_valve_2_status);  // Relay active LOW
}

// ######################################################################
unsigned char time_10_sec;
void checkTime() {
  time_10_sec++;
  if (time_10_sec >= 10) {
    time_10_sec = 0;
    Blynk.sendInternal("rtc", "sync");
  }

  //delay(dht.getMinimumSamplingPeriod());
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  soil_moisture = analogRead(MOISURE_READ);
  soil_moisture = map(soil_moisture, 1023, 0, 0, 1023);
  //delay(5000);
  flag_blynk_guage_update = 1;
}

// ######################################################################
void update_blynk_data(void) {
  if (flag_blynk_guage_update) {
    flag_blynk_guage_update = 0;
    Blynk.virtualWrite(BLYNK_TEMP, temperature);
    Blynk.virtualWrite(BLYNK_RH, humidity);
    Blynk.virtualWrite(BLYNK_SOIL_MOISTURE, soil_moisture);
  }

  if (flag_blynk_valve_1_update) {
    flag_blynk_valve_1_update = 0;
    Blynk.virtualWrite(BLYNK_VALVE_1, flag_valve_1_status);
  }

  if (flag_blynk_valve_2_update) {
    flag_blynk_valve_2_update = 0;
    Blynk.virtualWrite(BLYNK_VALVE_2, flag_valve_2_status);
  }
}

//

void sendSensor() {
  MQ2_Val = analogRead(MQ2);
  for (int x = 0; x < 100; x++) {
    MQ2_Val = MQ2_Val + analogRead(MQ2);
  }
  MQ2_Val = MQ2_Val / 100.0;

  Blynk.virtualWrite(V14, MQ2_Val);

  if (MQ2_Val > 600) {
    //Blynk.notify("Gas Detected!");
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(buzzer, LOW);
    Blynk.logEvent("gas","แจ้งเตือน!! ตรวจพบแก๊ส,ควัน ภายในห้องครัว");
    led.on();
  }

  else {
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
    digitalWrite(buzzer, HIGH);
    led.off();
  }
  //Serial.print("MQ2 Level: ");
  //Serial.println(MQ2_Val);
  //delay(500);
}
// ######################################################################
void setup() {
  // Debug console
  Serial.begin(9600);
  //
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, !toggleState_1);
  digitalWrite(RelayPin2, !toggleState_2);
  digitalWrite(RelayPin3, !toggleState_3);
  digitalWrite(RelayPin4, !toggleState_4);

  digitalWrite(wifiLed, LOW);

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);
  config3.setEventHandler(button3Handler);
  config4.setEventHandler(button4Handler);

  button1.init(SwitchPin1);
  button2.init(SwitchPin2);
  button3.init(SwitchPin3);
  button4.init(SwitchPin4);

  //Blynk.begin(auth, ssid, pass);
  WiFi.begin(ssid, pass);
  timer.setInterval(1000, checkBlynkStatus);  // check if Blynk server is connected every 2 seconds
  Blynk.config(auth);
  delay(100);

  if (!fetch_blynk_state) {
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
  }
  //
  //gas
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  timer.setInterval(1000L, sendSensor);



  //  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // DHT initialization
  dht.begin();

  pinMode(VALVE_1_OUT, OUTPUT);
  pinMode(VALVE_2_OUT, OUTPUT);
  pinMode(MOISURE_READ, INPUT);

  digitalWrite(VALVE_1_OUT, VALVE_OFF);
  digitalWrite(VALVE_2_OUT, VALVE_OFF);

  //
  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  //
  pinMode(PIR, INPUT);
  pinMode(Buzzer, OUTPUT);
  //
}

BLYNK_WRITE(V13) {
  Serial.println(param.asInt());
  Serial.println(Status);
  if (param.asInt()) {
    Status = 1;
    LCD.clear();
    LCD.print(1, 0, "On The System");
    LCD.print(4, 1, "<<Mode>>");
    LED.off();
  } else {
    Status = 0;
    LED.off();
    LCD.clear();
    LCD.print(1, 0, "Off The System");
    LCD.print(4, 1, "<<Mode>>");
    digitalWrite(Buzzer, LOW);
  }
}
// ######################################################################
unsigned long ms_buf;
void loop() {
  timer.run();
  Blynk.run();
  button1.check();
  button2.check();
  button3.check();
  button4.check();

  if (Status == 1) {
    if (digitalRead(PIR)) {
      LCD.clear();
      LCD.print(2, 0, "Some One Come");
      LCD.print(4, 1, "<<Danger>>");
      if (!PIRStatus) {
        PIRStatus = 1;
        Blynk.logEvent("pir","แจ้งเตือน!! ตรวจพบการเคลื่อนไหว");
        digitalWrite(Buzzer, LOW);
        //Blynk.notify("Some One Come");
        //       Line_Notify("Some One Come");
        LED.on();
        // delay(2000);
        LCD.clear();
        LCD.print(1, 0, "Everything OK ");
        LCD.print(4, 1, "<< OK >>");
        digitalWrite(Buzzer, HIGH);
        LED.off();
        PIRStatus = 0;
        LED.off();
      }
    }
  } else {
    LED.off();
    //Serial.println("off");
  }
  //


  unsigned long ms_dif = millis() - ms_buf;
  if (ms_dif >= 1000)  // 1 second
  {
    ms_buf = millis();
    checkTime();
  }

  fn_valve_mng();
  update_blynk_data();
  delay(50);

  //
  int ldrStatus = digitalRead(ldrPin);
  if (ldrStatus == 1) {
    digitalWrite(ledPin, LOW);
    //Serial.print("Its DARK, Trun on the LED : ");
    //Serial.println(ldrStatus);
    //delay(2000);
  } else {
    digitalWrite(ledPin, HIGH);
    //Serial.print("Its BRIGHT, Turn off the led : ");
    //Serial.println(ldrStatus);
    //delay(2000);
  }

  //
  //endloop
}
//
//
void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  switch (eventType) {
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      digitalWrite(RelayPin1, toggleState_1);
      toggleState_1 = !toggleState_1;
      Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
      break;
  }
}

void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  switch (eventType) {
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      digitalWrite(RelayPin2, toggleState_2);
      toggleState_2 = !toggleState_2;
      Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
      break;
  }
}

void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT3");
  switch (eventType) {
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      digitalWrite(RelayPin3, toggleState_3);
      toggleState_3 = !toggleState_3;
      Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
      break;
  }
}

void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT4");
  switch (eventType) {
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      digitalWrite(RelayPin4, toggleState_4);
      toggleState_4 = !toggleState_4;
      Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
      break;
  }
}
//