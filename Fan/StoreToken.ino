#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information

WiFiClient client;

// We limit this so you won't use all of your Temboo calls while testing
int maxCalls = 10;

// The number of times this Choreo has been run so far in this sketch
int calls = 0;

int outputPin = P2_5;

void setup() {
  Serial.begin(9600);
  
  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID);
    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }

  // Initialize condition pins
  pinMode(outputPin, OUTPUT);
  Serial.println("Setup complete.\n");
}

void loop() {
  if (calls < maxCalls) {
    Serial.println("Calling StoreToken Choreo...");
    runStoreToken();
    calls++;
  } else {
    Serial.println("Skipping to save Temboo calls. Adjust maxCalls as required.");
  }
  delay(30000);
}

void runStoreToken() {
  TembooChoreo StoreTokenChoreo(client);

  // Set Temboo account credentials
  StoreTokenChoreo.setAccountName(TEMBOO_ACCOUNT);
  StoreTokenChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  StoreTokenChoreo.setAppKey(TEMBOO_APP_KEY);

  // Set profile to use for execution
  StoreTokenChoreo.setProfile("Fan");
  // Identify the Choreo to run
  StoreTokenChoreo.setChoreo("/Library/Utilities/TokenStorage/StoreToken");

  // Run the Choreo
  unsigned int returnCode = StoreTokenChoreo.run(true);

  // A return code of zero means everything worked
  if (returnCode == 0) {
    while (StoreTokenChoreo.available()) {
      String name = StoreTokenChoreo.readStringUntil('\x1F');
      name.trim();

      String data = StoreTokenChoreo.readStringUntil('\x1E');
      data.trim();

      if (name == "Updated") {
        if (data.equalsIgnoreCase("true")) {
          digitalWrite(outputPin, HIGH);
        }
      }
    }
  }
  StoreTokenChoreo.close();
}