#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID        "13386e50-5384-11ea-8d77-2e728ce88125"
#define CHARACTERISTIC_UUID "133870f8-5384-11ea-8d77-2e728ce88125"
#define BLE_BROADCAST_NAME  "SmartSoleR"
#define BLE_DATA_SIZE        5

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
uint32_t value = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);
    
  // Create the BLE Device
  BLEDevice::init(BLE_BROADCAST_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  // Create the BLE Service
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                                       CHARACTERISTIC_UUID,
                                       BLECharacteristic::PROPERTY_READ   |
                                       BLECharacteristic::PROPERTY_WRITE  |
                                       BLECharacteristic::PROPERTY_NOTIFY
                                       );
  pCharacteristic->addDescriptor(new BLE2902());
  
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising* pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
}

void loop() {
  // NOTE: Do not exceed the 512-byte limit 
  uint16_t dataToTransmit[BLE_DATA_SIZE] = {};

  // TODO: Replace this with the values you actually want to send
  dataToTransmit[0] = 1 + value;
  dataToTransmit[1] = 20 + value;
  dataToTransmit[2] = 300 + value;
  dataToTransmit[3] = 4 + value;
  dataToTransmit[4] = 50 + value;
  dataToTransmit[5] = 600 + value;
  dataToTransmit[6] = 7 + value;

  // notify changed value
  if (deviceConnected) {
    pCharacteristic->setValue((uint8_t*)&dataToTransmit, sizeof(dataToTransmit));
    pCharacteristic->notify();
    value++;
  }
    // disconnecting
  if (!deviceConnected) {
     delay(500); // give the bluetooth stack the chance to get things ready
     pServer->startAdvertising(); // restart advertising
     Serial.println("start advertising");
  }
  
  delay(2500);
}
