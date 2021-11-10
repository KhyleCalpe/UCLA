/****************************************
 * Include Libraries
 ****************************************/
#include <WiFi.h>
#include <PubSubClient.h>
#include "BLEDevice.h"

#define WIFISSID "EXAMPLE_NAME" // Put your WifiSSID here
#define PASSWORD "EXAMPLE_PASSWORD" // Put your wifi password here
#define TOKEN "BBFF-XTVNYGlwTanmJM5Kgnl4NnVgJlBfDG" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "034533150679" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
                                           //it should be a random and unique ascii string and different from all other devices

// The remote service we wish to connect to.
static BLEUUID SERVICE_UUID_ACCELERATION("237ef00e-d2b3-498d-aa7f-4f089e05804b");
static BLEUUID SERVICE_UUID_PROXIMITY("47ba87b3-9767-4f01-8e69-310b39f8564c");
// The characteristic of the remote service we are interested in.
static BLEUUID CHAR_UUID_ACCELERATION("6d8a97f7-1630-4643-a283-d9ad2db349ee");
static BLEUUID CHAR_UUID_PROXIMITY("cbfd998b-fe64-483e-b712-aa0eeb878ba3");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristicAcceleration;
static BLERemoteCharacteristic* pRemoteCharacteristicProximity;
static BLEAdvertisedDevice* myDevice;

/****************************************
 * Define Constants
 ****************************************/
#define VARIABLE_LABEL_ACCELERATION "acceleration" // Assing the variable label
#define VARIABLE_LABEL_PROXIMITY "proximity" // Assing the variable label
#define DEVICE_LABEL "esp32" // Assig the device label

char mqttBroker[]  = "industrial.api.ubidots.com";
char payloadAcceleration[100];
char payloadProximity[100];
char topicAcceleration[150];
char topicProximity[150];
char strAcceleration[6];
char strProximity[3];

/****************************************
 * Auxiliary Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  Serial.write(payload, length);
  Serial.println(topic);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  int8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteServiceAcceleration = pClient->getService(SERVICE_UUID_ACCELERATION);
    if (pRemoteServiceAcceleration == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(SERVICE_UUID_ACCELERATION.toString().c_str());
      pClient->disconnect();
      return false;
    }
    BLERemoteService* pRemoteServiceProximity = pClient->getService(SERVICE_UUID_PROXIMITY);
    if (pRemoteServiceProximity == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(SERVICE_UUID_PROXIMITY.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristicAcceleration = pRemoteServiceAcceleration->getCharacteristic(CHAR_UUID_ACCELERATION);
    pRemoteCharacteristicProximity = pRemoteServiceProximity->getCharacteristic(CHAR_UUID_PROXIMITY);
    if (pRemoteCharacteristicAcceleration == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(CHAR_UUID_ACCELERATION.toString().c_str());
      pClient->disconnect();
      return false;
    }
    if (pRemoteCharacteristicProximity == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(CHAR_UUID_PROXIMITY.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristics");
    connected = true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(SERVICE_UUID_ACCELERATION) ) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks
/****************************************
 * Main Functions
 ****************************************/
void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFISSID, PASSWORD);

  Serial.print("Wait for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);  

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  int prevMillis = 0;
  
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  // While we are connected to a peer BLE Server, print the characteristics
  while (connected) {
    if (millis() - prevMillis >= 1000)
    {
      sprintf(topicAcceleration, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
      sprintf(topicProximity, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
      sprintf(payloadAcceleration, "%s", ""); // Cleans the payload
      sprintf(payloadProximity, "%s", ""); // Cleans the payload
      sprintf(payloadAcceleration, "{\"%s\":", VARIABLE_LABEL_ACCELERATION); // Adds the variable label
      sprintf(payloadProximity, "{\"%s\":", VARIABLE_LABEL_PROXIMITY); // Adds the variable label      
      if(pRemoteCharacteristicAcceleration->canNotify()) {
        if(pRemoteCharacteristicAcceleration->canRead()) {
          std::string value = pRemoteCharacteristicAcceleration->readValue();
          float accelerationX = *(float*)(value.data());
          dtostrf(accelerationX, 4, 2, strAcceleration);
          sprintf(payloadAcceleration, "%s {\"value\": %s}}", payloadAcceleration, strAcceleration); // Adds the value
        }
      }
      if(pRemoteCharacteristicProximity->canNotify()) {
        if(pRemoteCharacteristicProximity->canRead()) {
          std::string value = pRemoteCharacteristicProximity->readValue();
          int proximity = *(uint8_t*)value.data();
          dtostrf(proximity, 3, 0, strProximity);
          sprintf(payloadProximity, "%s {\"value\": %s}}", payloadProximity, strProximity); // Adds the value
        } 
      }
      
      Serial.print(topicAcceleration);
      Serial.println(payloadAcceleration);
      Serial.print(topicProximity);
      Serial.println(payloadProximity);
      
      Serial.println("Publishing data to Ubidots Cloud");
      client.publish(topicAcceleration, payloadAcceleration);
      client.publish(topicProximity, payloadProximity);
      client.loop();
      prevMillis = millis();
    }
  } 
}
