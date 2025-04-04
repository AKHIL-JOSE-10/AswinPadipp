#include <WiFi.h>
#include <HTTPClient.h>

#define WIFI_SSID "vivo1915"     // Set your WiFi SSID
#define WIFI_PASSWORD "8137087053" // Set your WiFi Password
#define SERVER_URL "http://192.168.248.191:5000/update" // Replace with your server URL

#define TDS_PIN 34         // ADC pin for TDS sensor
#define TURBIDITY_PIN 35   // ADC pin for Turbidity sensor
#define VREF 3.3           // ESP32 ADC reference voltage
#define ADC_RES 4095       // 12-bit ADC resolution
#define NUM_SAMPLES 10     // Number of samples for averaging

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");

    // Enable full 0-3.3V range for ADC readings
    analogSetAttenuation(ADC_11db);
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        float tdsValue = readTDS();
        float turbidityValue = readTurbidity();

        // Create JSON payload
        String payload = "{\"tds\":" + String(tdsValue, 2) + 
                         ", \"turbidity\":" + String(turbidityValue, 2) + "}";

        HTTPClient http;
        http.begin(SERVER_URL);
        http.addHeader("Content-Type", "application/json");

        // Send HTTP POST request with JSON data
        int httpResponseCode = http.POST(payload);

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        if (httpResponseCode > 0) {
            Serial.println("Server Response: " + http.getString());
        } else {
            Serial.println("Error sending POST request");
        }

        http.end();
    } else {
        Serial.println("WiFi Not Connected");
    }

    delay(5000);  // Send data every 5 seconds
}

// Function to read and average TDS sensor data
float readTDS() {
    int sumADC = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        int adcVal = analogRead(TDS_PIN);
        sumADC += adcVal;
        Serial.print("Raw ADC Reading (TDS): ");
        Serial.println(adcVal);  // Debugging
        delay(10);
    }

    int rawADC = sumADC / NUM_SAMPLES;
    float voltage = rawADC * (VREF / ADC_RES);
    float tdsValue = (133.42 * voltage * voltage * voltage - 
                      255.86 * voltage * voltage + 
                      857.39 * voltage) * 0.5;

    Serial.print("Avg ADC: "); Serial.print(rawADC);
    Serial.print(" | Voltage: "); Serial.print(voltage, 3);
    Serial.print("V | TDS: "); Serial.println(tdsValue, 2);
    
    return tdsValue;
}

// Read and process Turbidity sensor data
float readTurbidity() {
    int sumADC = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sumADC += analogRead(TURBIDITY_PIN);
        delay(10);
    }

    int rawADC = sumADC / NUM_SAMPLES;
    float voltage = rawADC * (VREF / ADC_RES);  // Convert ADC to voltage
    Serial.print("Avg ADC Turbidity: "); Serial.print(rawADC);
    Serial.print(" | Voltage: "); Serial.print(voltage, 3);
    return voltage * 100;  // Convert to an arbitrary scale (adjust as needed)
}
