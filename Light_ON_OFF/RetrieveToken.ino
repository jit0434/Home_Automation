#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information

WiFiClient client;

int calls = 1;   // Execution count, so this doesn't run forever
int maxCalls = 10;   // Maximum number of times the Choreo should be executed

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

  Serial.println("Setup complete.\n");
}

void loop() {
  if (calls <= maxCalls) {
    Serial.println("Running RetrieveToken - Run #" + String(calls++));

    TembooChoreo RetrieveTokenChoreo(client);

    // Invoke the Temboo client
    RetrieveTokenChoreo.begin();

    // Set Temboo account credentials
    RetrieveTokenChoreo.setAccountName(TEMBOO_ACCOUNT);
    RetrieveTokenChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    RetrieveTokenChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set profile to use for execution
    RetrieveTokenChoreo.setProfile("Light");

    // Identify the Choreo to run
    RetrieveTokenChoreo.setChoreo("/Library/Utilities/TokenStorage/RetrieveToken");

    // Run the Choreo; when results are available, print them to serial
    // Boolean input to tell library to use HTTPS
    RetrieveTokenChoreo.run(true);
    
    while(RetrieveTokenChoreo.available()) {
      char c = RetrieveTokenChoreo.read();
      Serial.print(c);
    }
    RetrieveTokenChoreo.close();
  }

  Serial.println("\nWaiting...\n");
  delay(30000); // wait 30 seconds between RetrieveToken calls
}