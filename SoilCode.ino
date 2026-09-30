#include <Ethernet.h>
#include <SoftwareSerial.h>

// RS-485 control pins
#define RE 6
#define DE 7
#define RO 8
#define DI 9

// SoftwareSerial for Quectel M10F communication (RX, TX)
SoftwareSerial mod(2, 3); // RX = Pin 2, TX = Pin 3 (change pins if needed)

const byte temp[] = {0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x75, 0xcf};
const byte mois[] = {0x01, 0x03, 0x00, 0x12, 0x00, 0x01, 0x24, 0x0F};
const byte econ[] = {0x01, 0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xce};
const byte ph[] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0b};
const byte nitro[] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x01, 0xE4, 0x0C};
const byte phos[] = {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[30];

float envhumidity = 0.0, envtemperature = 0.0, soil_ph = 0.0, soil_mois = 0.0, soil_temp = 0.0;
byte val1 = 0, val2 = 0, val3 = 0, val4 = 0, val5 = 0, val6 = 0, val7 = 0;
String cmd="";
void setup() {
  Serial.begin(9600);                 // Start hardware serial for debugging
  mod.begin(9600);                    // Start SoftwareSerial communication with Quectel M10F
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  // Put RS-485 into receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  delay(3000);
}

void loop() {
  // Send AT command to Quectel module
  mod.println("AT");
  cmd="";
  val1 = moisture();
  soil_mois = val1 / 1.8;
  delay(1000);
  soil_temp = temperature() / 10.0;
  delay(1000);
  val3 = econduc();
  delay(1000);
  val4 = phydrogen() / 25;
  soil_ph = val4;
  delay(1000);
  val5 = nitrogen();
  delay(1000);
  val6 = phosphorous();
  delay(1000);
  val7 = potassium();
  delay(1000);

/* Serial.print(soil_mois); Serial.print(",");
  delay(1000);
  Serial.print(soil_temp); Serial.print(",");
  delay(1000);
  Serial.print(val3); Serial.print(",");
  delay(1000);
  Serial.print(soil_ph); Serial.print(",");
  delay(1000);
  Serial.print(val5); Serial.print(",");
  delay(1000);
  Serial.print(val6); Serial.print(",");
  delay(1000);
  Serial.print(val7); Serial.print(",");
  Serial.println(); */

cmd += soil_mois;
cmd += ",";
cmd += soil_temp;
cmd += ",";
cmd += val3;
cmd += ",";
cmd += soil_ph;
cmd += ",";
cmd += val5;
cmd += ",";
cmd += val6;
cmd += ",";
cmd += val7;
cmd += "\n";
Serial.print(cmd);


  delay(3000);

  // Read and print response from the Quectel module
  while (mod.available()) {
    Serial.write(mod.read());  // Display the response on the Serial Monitor
  }
}

// The following functions remain the same as in your original code for handling RS-485 communication.
byte moisture() {
  mod.flush();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);
  for (uint8_t i = 0; i < sizeof(mois); i++) mod.write(mois[i]);
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(200);
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
  }
  return values[4];
}

byte temperature() {
  mod.flush();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);
  for (uint8_t i = 0; i < sizeof(temp); i++) mod.write(temp[i]);
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(200);
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
  }
  return values[3] << 8 | values[4];
}

byte econduc() {
  mod.flush();

  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  for (uint8_t i = 0; i < sizeof(econ); i++) mod.write(econ[i]);

  mod.flush();

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  delay(200);

  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
  }
  return values[4];
}

byte phydrogen() {
  mod.flush();

  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  for (uint8_t i = 0; i < sizeof(ph); i++) mod.write(ph[i]);

  mod.flush();

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  delay(200);

  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
  }
  return values[4];
}

byte nitrogen() {
  mod.flush();

  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  for (uint8_t i = 0; i < sizeof(nitro); i++) mod.write(nitro[i]);

  mod.flush();

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  delay(200);

  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
  }
  return values[4];
}

byte phosphorous() {
  mod.flush();

  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  for (uint8_t i = 0; i < sizeof(phos); i++) mod.write(phos[i]);

  mod.flush();

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  delay(200);

  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
  }
  return values[4];
}

byte potassium() {
  mod.flush();

  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  for (uint8_t i = 0; i < sizeof(pota); i++) mod.write(pota[i]);

  mod.flush();

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  delay(200);

  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
  }
  return values[4];
}
