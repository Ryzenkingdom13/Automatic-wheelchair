#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

// ================= WIFI =================
const char* ssid = "Firmware";
const char* password = "Solutions@12345";

// ================= SERVER =================
String server = "http://esskay-012024.live/ananuniversity_robo/";

// ================= MOTOR =================
#define IN1 4
#define IN2 5
#define IN3 18
#define IN4 19

// ================= SPEED =================
#define ENA 21
int speedValue = 200;

// ================= ULTRASONIC =================
#define TRIG 22
#define ECHO 23

// ================= GPS =================
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

// ================= VC02 =================
HardwareSerial VC02(2);

// ================= DEFAULT GPS =================
float defaultLat = 11.042965;
float defaultLon = 76.886709;

// ================= TIMER =================
unsigned long lastGPS = 0;
unsigned long lastCMD = 0;

// ================= FLAG =================
int a = 0;

// ================= SPEED =================
void setSpeed(int spd) {
  ledcWrite(ENA, spd);
}

// ================= MOTOR =================
void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void forward() {
  setSpeed(speedValue);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void reverse() {
  setSpeed(speedValue);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  setSpeed(speedValue);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  setSpeed(speedValue);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// ================= WIFI =================
void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

// ================= GPS SEND =================
void sendGPS(String lat, String lon) {
  HTTPClient http;
  http.begin(server + "send_gps.php?lat=" + lat + "&lon=" + lon);
  http.GET();
  http.end();
}

// ================= ULTRASONIC =================
long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  long distance = duration * 0.034 / 2;

  return distance;
}

// ================= VC02 =================
void readVC02() {
  while (VC02.available() >= 2) {

    byte header = VC02.read();
    byte command = VC02.read();

    if (header == 0x20) {

      if (command == 0xA0) { forward(); a = 1; }
      else if (command == 0xA1) { reverse(); a = 1; }
      else if (command == 0xA2) { left(); a = 1; }
      else if (command == 0xA3) { right(); a = 1; }
      else if (command == 0xA4) { stopMotor(); a = 0; }

      Serial.println("VC02 Command");
    }
  }
}

// ================= WEB =================
void getCommand() {
  HTTPClient http;
  http.begin(server + "get_command.php");

  if (http.GET() > 0) {
    String payload = http.getString();

    int comma = payload.indexOf(',');

    String cmd = payload.substring(0, comma);
    int emergency = payload.substring(comma + 1).toInt();

    if (emergency == 1) {
      stopMotor();
      forward();
      delay(5000);
      left();
      delay(2000);
      forward();
      return;
    }

    if (cmd == "FORWARD") forward();
    else if (cmd == "REVERSE") reverse();
    else if (cmd == "LEFT") left();
    else if (cmd == "RIGHT") right();
    else if (cmd == "STOP" && a == 0) stopMotor();
  }

  http.end();
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  ledcAttach(ENA, 5000, 8);
  setSpeed(speedValue);

  connectWiFi();

  gpsSerial.begin(9600, SERIAL_8N1, 26, 27);
  VC02.begin(9600, SERIAL_8N1, 16, 17);

  Serial.println("System Ready...");
}

// ================= LOOP =================
void loop() {

  // ?? OBSTACLE DETECTION
  long dist = getDistance();

  Serial.print("Distance: ");
  Serial.println(dist);

  if (dist > 0 && dist < 20) {
    Serial.println("Obstacle! STOP");
    stopMotor();
    return;
  }

  // VC02 CONTROL
  readVC02();

  // GPS READ
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // SEND GPS
  if (millis() - lastGPS > 3000) {
    lastGPS = millis();

    if (gps.location.isValid()) {
      sendGPS(
        String(gps.location.lat(), 6),
        String(gps.location.lng(), 6)
      );
    } else {
      sendGPS(
        String(defaultLat, 6),
        String(defaultLon, 6)
      );
    }
  }

  // WEB CONTROL
  if (millis() - lastCMD > 2000) {
    lastCMD = millis();
    getCommand();
  }
}