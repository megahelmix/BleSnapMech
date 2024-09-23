/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "bleCharas.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "59d2855d-f4d3-4da7-8da1-697d057d25d1"
#define CHARACTERISTIC_UUID "7e2bd36e-fb6e-4ce8-93ab-4410194bd15e"

BLECharacteristic *pChara;

const char* charaNames[BLE_CHARAS] =
{
  "EYES_X",
  "EYES_Y",
  "LID_POS",
  "BLINK",
};

chara_data_t charaData;

uint8_t charaBuf[RAW_LENGTH];
chara_data_t charaData_sav =
{
  charaBuf
};

void setupBLE() {
  log_i("Starting BLE work!");

  BLEDevice::init("Animatronics#1");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pChara = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);// | BLECharacteristic::PROPERTY_NOTIFY);
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  log_i("Characteristic defined!");
}

void loopBleNormal()
{
  //uint8_t* tmpBuf;
  char dispBuf[128];
  short tempVal;
  charaData.raw = pChara->getData();
  for(int idx=0; idx<BLE_CHARAS; idx++)
  {
    tempVal = charaData.normal[idx];
    if(tempVal != charaData_sav.normal[idx])
    {
      charaData_sav.normal[idx] = tempVal;
      sprintf(dispBuf, "%s: %i", charaNames[idx], charaData_sav.normal[idx]);
      log_i(dispBuf);
    }
  }
}

void writeBleData(uint8_t* data, size_t len)
{
  pChara->setValue(data, len);
}
