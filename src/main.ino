#include <Arduino.h>
#include <M5Lite.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 4;
BLEScan *pBLEScan;

const char *uuid = "0000fd6f-0000-1000-8000-00805f9b34fb";

int counterDevices = 0;
bool dark = false;
bool display = true;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        if (advertisedDevice.haveServiceUUID())
        {
            if (strncmp(advertisedDevice.getServiceUUID().toString().c_str(), uuid, 36) == 0)
            {
                int rssi = advertisedDevice.getRSSI();
                //Serial.print("RSSI: ");
                //Serial.println(rssi);
                //Serial.print("ADDR: ");
                //Serial.println(advertisedDevice.getAddress().toString().c_str());
                //Serial.println("Found!");
                counterDevices++;
            }
        }
    }
};

void drawScreen()
{
    M5Lite.Lcd.fillScreen(BLACK);

    M5Lite.Lcd.setTextSize(2);
    M5Lite.Lcd.setCursor(0, 0);
    M5Lite.Lcd.setTextColor(RED);
    M5Lite.Lcd.print("CORONA Counter\n");

    M5Lite.Lcd.setCursor(0, M5Lite.Lcd.height() - 100);
    M5Lite.Lcd.setTextSize(7);
    M5Lite.Lcd.setTextColor(WHITE);
    M5Lite.Lcd.printf(" %2d", counterDevices);

    M5Lite.Lcd.setTextSize(1);
    M5Lite.Lcd.setCursor(0, M5Lite.Lcd.height() - 10);
    M5Lite.Lcd.printf("Bat:%5.1fV ", M5Lite.Axp.GetBatVoltage());
    M5Lite.Lcd.printf("Charge:%5.1f\n", M5Lite.Axp.GetBatCurrent());
}

void dScan(void *pvParameters)
{
    while (true)
    {
        counterDevices = 0;
        BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
        //Serial.print("Devices found: ");
        //Serial.println(counterDevices);
        //Serial.println("Scan done!");
        pBLEScan->clearResults();
        drawScreen();
    }
}

void setup()
{
    M5Lite.begin();
    Serial.begin(115200);
    Serial.println("Scanning...");
    M5Lite.Lcd.setRotation(3);
    M5Lite.Axp.ScreenBreath(10);
    M5Lite.Lcd.fillScreen(BLACK);

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(5000);
    pBLEScan->setWindow(4999);

    xTaskCreatePinnedToCore(dScan, "Scan", 4096, NULL, 3, NULL, 1);

    display = true;
}

void loop()
{
    M5Lite.update();
    if (M5Lite.BtnA.wasReleased())
    {
        if (display)
        {
            //M5Lite.Lcd.setBrightness(0);
            M5Lite.Axp.ScreenBreath(0); 
            display = false;
        }
        else
        {
            //M5Lite.Lcd.setBrightness(10);
            M5Lite.Axp.ScreenBreath(10); 
            display = true;
        }
        //M5Lite.Axp.PowerOff();
        //M5Lite.Lcd.writecommand(ST7735_DISPOFF);
        //  M5Lite.Lcd.setBrightness(0);
    }
}