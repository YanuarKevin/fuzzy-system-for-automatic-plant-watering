#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 33
#define DHTTYPE DHT11
#define SOIL_MOISTURE_PIN 35
#define RAIN_SENSOR_PIN 32
#define LDR_PIN 34
#define RELAY_PIN 27

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// PENYIMPANAN DATA PEMBACAN SENSOR
float soilMoisture = 0.0;
float rainSensor = 0.0;
float lightIntensity = 0.0;
float temperature = 0.0;
float humidity = 0.0;
float irrigationDuration = 0.0;

// MILLIS PEMBACAAN SENSOR
unsigned long previousSensorMillis = 0;
unsigned long previousIrrigationMillis = 0;
const long sensorInterval = 1000; // 1 second interval for sensor reading

// NILAI FUZZY
float soilDry = 600.0;     // Sangat kering
float soilSlightlyDry = 450.0; // Sedikit kering
float soilWet = 300.0;     // Basah
float soilVeryWet = 150.0; // Sangat basah
float highLight = 800.0;   // Terang
float mediumLight = 500.0; // Sedang
float lowLight = 200.0;    // Gelap
float highTemp = 35.0;     // Suhu tinggi
float moderateTemp = 25.0; // Suhu sedang
float lowTemp = 15.0;      // Suhu rendah
float highHumidity = 80.0; // Kelembaban tinggi
float moderateHumidity = 60.0; // Kelembaban sedang
float lowHumidity = 40.0;  // Kelembaban rendah

byte termometer[8] = {
  0b00100,
  0b01010,
  0b01010,
  0b01010,
  0b01110,
  0b11111,
  0b11111,
  0b01110
};

byte derajat[8] = {
  0b00110,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte air[8] = {
  0b00100,
  0b00100,
  0b01110,
  0b11111,
  0b11111,
  0b01110,
  0b00000,
  0b00000
};

byte matahari[8] = {
  0b00100,
  0b10101,
  0b01110,
  0b11111,
  0b01110,
  0b10101,
  0b00100,
  0b00000
};

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, termometer);
  lcd.createChar(1, derajat);
  lcd.createChar(2, air);
  lcd.createChar(3, matahari);

  lcd.setCursor(2, 0);
  lcd.print("COPYRIGHT BY");
  lcd.setCursor(2, 1);
  lcd.print("KELOMPOK 100");
  delay(5000);
  lcd.clear();
  
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // JAWA ADALAH KOENCI
}

void loop() {
  unsigned long currentMillis = millis();

  // PEMBACAAN SENSOR TIAP 1 DETUK
  if (currentMillis - previousSensorMillis >= sensorInterval) {
    previousSensorMillis = currentMillis;
    readSensors();
    displaySerial();
    checkIrrigation();
  }
  updateLCD();
}

void readSensors() {
  soilMoisture = analogRead(SOIL_MOISTURE_PIN); // KELEMBABAN TANAH
  lightIntensity = analogRead(LDR_PIN); // INTENSITAS CAHAYA
  temperature = dht.readTemperature();B // DHT11
  humidity = dht.readHumidity();
  rainSensor = digitalRead(RAIN_SENSOR_PIN); // SENSOR HUJAN
}

void displaySerial() {
  Serial.print("Kelembaban Tanah: ");
  Serial.println(soilMoisture);
  Serial.print("Intensitas Cahaya: ");
  Serial.println(lightIntensity);
  Serial.print("Suhu Ruang: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("Kelembaban Ruang: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.print("HUJAN: ");
  Serial.println(rainSensor ? "Yes" : "No");
  Serial.println("-----------------------------");
}

void updateLCD() {
  lcd.setCursor(0, 0);
  lcd.write(byte(2));
  lcd.print("T:");
  lcd.print(soilMoisture);
  lcd.print("%");

  lcd.setCursor(10, 0);
  lcd.write(byte(3));
  lcd.print("M:");
  lcd.print(lightIntensity);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.print("U:");
  lcd.print(temperature);
  lcd.write(byte(1));
  lcd.print("C");

  lcd.setCursor(10, 1);
  lcd.write(byte(2));
  lcd.print("U:");
  lcd.print(humidity);
  lcd.print("%");
  
  delay(60000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("HUJAN: ");
  lcd.print("25");
  lcd.print(rainSensor ? "IYA" : "TIDAK");

  lcd.setCursor(0, 1);
  lcd.print("PENYIRAMAN: ");
  lcd.print(irrigationDuration * 1000);
  lcd.print("S");

  delay(30000);
  lcd.clear();

}

// Function to check irrigation condition
void checkIrrigation() {
  // Calculate irrigation duration based on fuzzy logic
  irrigationDuration = calculateIrrigationDuration(soilMoisture, rainSensor, lightIntensity, temperature, humidity);

  // Control relay based on fuzzy logic
  if (rainSensor == LOW && soilMoisture > soilDry) {
    digitalWrite(RELAY_PIN, LOW); // MULAI PENYIRAMAN
    delay(irrigationDuration * 1000); // DURASI PENYIRAMAN (S)
    digitalWrite(RELAY_PIN, HIGH); // WES BAR
  }
}

// Function to calculate irrigation duration based on fuzzy logic
float calculateIrrigationDuration(float soil, float rain, float light, float temp, float hum) {
  float duration = 0.0;

  // Fuzzy logic rules for irrigation duration
  if (soil > soilDry && rain == LOW && light < mediumLight && temp > moderateTemp && hum < moderateHumidity) {
    duration = 25.0; // Maximum irrigation needed
  } else if (soil > soilDry && rain == LOW && light < highLight && temp > lowTemp && hum < highHumidity) {
    duration = 20.0; // High irrigation
  } else if (soil > soilSlightlyDry && rain == LOW && light < mediumLight && temp > lowTemp) {
    duration = 15.0; // Medium-high irrigation
  } else if (soil > soilWet && rain == LOW && light < lowLight) {
    duration = 10.0; // Medium irrigation
  } else if (soil > soilWet && rain == LOW && light < mediumLight) {
    duration = 5.0;  // Low irrigation
  } else if (soil > soilVeryWet || rain == HIGH) {
    duration = 0.0;  // No irrigation
  }

  return duration;
}
