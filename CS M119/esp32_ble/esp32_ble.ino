#include "BLEDevice.h"
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

void setup() {
  Serial.begin(9600);
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
} // End of setup.


// This is the Arduino main loop function.
void loop() {

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
      if(pRemoteCharacteristicAcceleration->canNotify())
      {
        if(pRemoteCharacteristicAcceleration->canRead()) {
          std::string value = pRemoteCharacteristicAcceleration->readValue();
          float val = *(float*)(value.data());
          Serial.print("Acceleration in the x direction: ");
          Serial.println(val);
        }
      }
      if(pRemoteCharacteristicProximity->canNotify()) {
        if(pRemoteCharacteristicProximity->canRead()) {
          std::string value = pRemoteCharacteristicProximity->readValue();
          Serial.print("Proximity: ");
          Serial.println(*(uint8_t*)value.data());
        } 
      }
      prevMillis = millis();
    }
  }
  
} // End of loop
