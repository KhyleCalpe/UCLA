#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>  // Accelerometer
#include <Arduino_APDS9960.h> // Proximity
//----------------------------------------------------------------------------------------------------------------------
// BLE UUIDs
//----------------------------------------------------------------------------------------------------------------------
#define BLE_UUID_SERVICE_PROXIMITY      "47ba87b3-9767-4f01-8e69-310b39f8564c"
#define BLE_UUID_PROXIMITY              "cbfd998b-fe64-483e-b712-aa0eeb878ba3"
//----------------------------------------------------------------------------------------------------------------------
// BLE
//----------------------------------------------------------------------------------------------------------------------
BLEService proximityService( BLE_UUID_SERVICE_PROXIMITY );
BLEIntCharacteristic proximityCharacteristic( BLE_UUID_PROXIMITY, BLERead | BLENotify );

void setup()
{
  Serial.begin( 9600 );
  IMU.begin();
  APDS.begin();
  setupBleMode();
} // setup

void loop()
{
  static long previousMillis = 0;
  bool check_p = false;
  float accelerationX, accelerationY, accelerationZ;
  int proximity;
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();
  if ( central )
  {
    Serial.print( "Connected to central: " );
    Serial.println( central.address() );
    while ( central.connected() )
    {
      if ( millis() - previousMillis >= 1000 )
      {
        if ( APDS.proximityAvailable() && !check_p)
        {
          proximity = APDS.readProximity();
          Serial.print("proximity: ");
          Serial.println(proximity);
          proximityCharacteristic.writeValue( proximity );
          Serial.println( proximity );
          check_p = true;
        }
        if (check_p)
        {
          previousMillis = millis();
          check_a = check_p = false;
        }
      } // if every second
    } // while connected
    Serial.print( F( "Disconnected from central: " ) );
    Serial.println( central.address() );
  } // if central
} // loop

bool setupBleMode()
{
  if ( !BLE.begin() )
  {
    return false;
  }
  // set advertised local name and service UUID:
  BLE.setDeviceName( "Arduino Nano 33 BLE" );
  BLE.setLocalName( "Arduino Nano 33 BLE" );
  BLE.setAdvertisedService( accelerationService );
  // BLE add characteristics
  proximityService.addCharacteristic( proximityCharacteristic );
  // add service
  BLE.addService( proximityService );
  // set the initial value for the characeristic:
  proximityCharacteristic.writeValue( 0 );
  // start advertising
  BLE.advertise();
  return true;
}
