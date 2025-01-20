#include <Wire.h>
#include <MAX30100_PulseOximeter.h>
#include <Wire.h>
#include <ADXL345.h>
#include <SoftwareSerial.h>

#define SENSOR_READ_INTERVAL 1000   // Time interval for sensor readings in ms
#define THRESHOLD_MOVEMENT 10       // Threshold for detecting agitation based on accelerometer values
#define THRESHOLD_DECIBEL 500       // Threshold for audio decibel level

// Create instances for sensors
MAX30100_PulseOximeter pox; // Heart rate sensor
ADXL345 accel;              // Accelerometer
SoftwareSerial btSerial(10, 11); // Bluetooth Serial: RX, TX

// Variables to store sensor data
float heartRate = 0.0;
int heartRateValue = 0;
int SpO2Value = 0;
float ax = 0, ay = 0, az = 0; // Accelerometer data
int audioLevel = 0;            // Microphone audio level

void setup() {
  // Start serial communication for debugging and Bluetooth
  Serial.begin(9600);
  btSerial.begin(9600);         // Bluetooth communication

  // Initialize MAX30100 sensor
  if (!pox.begin()) {
    Serial.println("MAX30100 not detected!");
    while (1);
  }
  
  // Initialize ADXL345 accelerometer
  if (!accel.begin()) {
    Serial.println("ADXL345 not detected!");
    while (1);
  }

  // Set up accelerometer range
  accel.setRange(ADXL345_RANGE_16G);
  
  // Set up MAX30100 sensor
  pox.setIRLedCurrent(MAX30100_LED_CURRENT_50MA);
  pox.begin();
  
  // Set up timekeeping display (use an OLED or LCD for real watch functionality)
  // Timekeeping code is placeholder for clock functionality.
}

void loop() {
  // Update the heart rate and SpO2 readings
  pox.update();
  heartRate = pox.getHeartRate();
  SpO2Value = pox.getSpO2();
  
  // Get accelerometer data (detect agitation based on movement)
  accel.getAcceleration(&ax, &ay, &az);
  
  // Get audio decibel level (detect environmental noise level)
  audioLevel = analogRead(A0);  // Read audio input from mic sensor
  
  // Detect if anxiety (based on sensor thresholds)
  bool anxietyDetected = false;
  if (heartRate > 100 || audioLevel > THRESHOLD_DECIBEL || abs(ax) > THRESHOLD_MOVEMENT || abs(ay) > THRESHOLD_MOVEMENT || abs(az) > THRESHOLD_MOVEMENT) {
    anxietyDetected = true;
  }

  // Build data packet to send to the mobile app
  String data = "HR:" + String(heartRate) + ",SpO2:" + String(SpO2Value) + ",AudioLevel:" + String(audioLevel) + ",AnxietyDetected:" + String(anxietyDetected);
  
  // Send data over Bluetooth
  btSerial.println(data);

  // Output the data for debugging
  Serial.println(data);
  
  // Display time on the watch (use an appropriate timekeeping method)
  // For example, use an RTC (real-time clock) module or Arduino's millis() to keep track of time
  
  delay(SENSOR_READ_INTERVAL);  // Delay before the next read cycle
}
