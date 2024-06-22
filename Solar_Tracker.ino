#define BLYNK_TEMPLATE_ID "TMPL3e_tSJlSb"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "4VlvkrTiBwoqUWvsWobMp9eC6GP6eNMJ"
#define BLYNK_PRINT Serial
#define NOTIFICATION_PIN V5
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <DHT.h>
char auth[] = "4VlvkrTiBwoqUWvsWobMp9eC6GP6eNMJ"; // Blynk authentication token
char ssid[] = "lakshjogi"; // WiFi network name
char pass[] = "81254548"; // WiFi password
BlynkTimer timer;
// Solar Tracker Variables
Servo myservo; // Create servo object to control a servo
int ldrPin1 = 17; // LDR 1 connected to analog pin A1
int ldrPin2 = 18; // LDR 2 connected to analog pin A2
int ldrValue1, ldrValue2; // Variables to store LDR values
int threshold = 10; // Threshold for determining "equal" light intensity
// DHT Sensor Variables
#define DHTPIN D4 // Pin to which the DHT sensor is connected
#define DHTTYPE DHT11 // DHT 11 sensor type
DHT dht(DHTPIN, DHTTYPE);
// Soil Moisture and Pump Control Variables
const int moistureSensorPin = A0; // Soil moisture sensor connected to analog pin A0
const int relayPin = D5; // Relay is connected to pin D5
const int pumpRunTime = 500; // Pump run time in milliseconds
const int dripInterval = 1000; // Time between drip cycles in milliseconds
const int moistureThreshold = 1000;// Maximum soil moisture threshold
const float tempThreshold = 30.0; // Temperature threshold in Celsius
bool pumpRunning = false; // Flag to track if pump is running
void setup() {
Serial.begin(9600); // Initialize serial communication
Blynk.begin(auth, ssid, pass);
dht.begin(); // Initialize the DHT sensor
pinMode(relayPin, OUTPUT); // Set the relay pin as an output
pinMode(moistureSensorPin, INPUT); // Set the moisture sensor pin as input
myservo.attach(D1); // Attaches the servo on pin D1 to the servo object
// Setup timer for sensor readings

43

timer.setInterval(5000L, sendSensorData); // Update sensor data every 5 seconds
}
void loop() {
Blynk.run();
timer.run();
// Solar Tracker Logic
ldrValue1 = analogRead(ldrPin1);
ldrValue2 = analogRead(ldrPin2);
if (abs(ldrValue1 - ldrValue2) <= threshold) {
rotateServo(90); // Neutral position
}
else if (ldrValue1 > ldrValue2) {
rotateServo(0); // Towards LDR1
} else {
rotateServo(180); // Towards LDR2
}
// Soil Moisture and Temperature Logic
int moistureLevel = analogRead(moistureSensorPin);
float temperature = dht.readTemperature();
if (isnan(temperature)) {
Serial.println("Failed to read from DHT sensor!");
} else {
if (temperature > tempThreshold && moistureLevel < moistureThreshold) {
if (!pumpRunning) {
digitalWrite(relayPin, HIGH);
pumpRunning = true;
}
} else {
if (pumpRunning) {
digitalWrite(relayPin, LOW);
pumpRunning = false;
}
}
}
delay(1000); // Delay for stability
}
// Function to rotate servo to a specific angle
void rotateServo(int angle) {
myservo.write(angle);
delay(500); // Allow servo to reach the position
}
void sendSensorData() {
// Read temperature and humidity
float temperature = dht.readTemperature(); // Read temperature in Celsius
float humidity = dht.readHumidity(); // Read humidity

44

// Read soil moisture
int soilMoisture = analogRead(moistureSensorPin);
// Convert analog reading to percentage (assuming 0 as dry and 1023 as wet)
int moisturePercentage = map(soilMoisture, 0, 1023, 100, 0);
// Send sensor data to Blynk app
Blynk.virtualWrite(V0, temperature); // V0 is virtual pin for temperature gauge
Blynk.virtualWrite(V1, humidity); // V1 is virtual pin for humidity gauge
Blynk.virtualWrite(V2, moisturePercentage); // V2 is virtual pin for soil moisture gauge
// Check for low soil moisture
if (moisturePercentage < 30) {
Blynk.virtualWrite(NOTIFICATION_PIN, "Low soil moisture detected!"); // Send notification
}
}