#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
    OledDisplay
    -boot animation
    -esp status
    -menu system
    -master-node data display
    -auto dim on-off
*/

class OledDisplay{
public:

    /* ======= ENUMS ======== */ 
    enum class Screen{
        BOOT,
        STATUS,
        MENU,
        MASTER_DATA,
        SLAVE_DATA,
        SETTINGS,
        SLEEP
    };



    enum class NetState : uint8_t{
        DISCONNECTED = 0,
        CONNECTING,
        CONNECTED,
        ERROR
    };




    /* ====== STRUCTS ===== */
    struct NetInfo{
        NetState state = NetState::DISCONNECTED;
        int8_t rssi = -100;
        uint32_t packetsRx = 0;
        uint32_t lastPacketMs = 0;
    };

    struct BrightnessControl{
        uint8_t level;      // 0 - 255
        bool autoMode;
    };

    

    struct DisplayText{
        const char* label;
        const char* text;
    };




    /* ===== CONSTRUCTOR ===== */
    OledDisplay(uint8_t width, uint8_t height, uint8_t i2cAddress = 0x3C);




    /* ======================= LIFE CYCLE ===================== */
    bool begin(TwoWire& wire = Wire);
    void update();                  // se apleaza in loop
    void refresh();                 // refresh fortat
    



    /* ===== SCREENS ===== */
    void setScreen(Screen screen);
    Screen getCurrentScreen() const;



    /* ===== BOOT ===== */
    void playBootAnimation();




    /* ===== ESP STATS ===== */
    void setNetInfo(const NetInfo& info);
    void drawNetStatus();



    /* ===== MENU ===== */
    void nextMenuItem();
    void previousMenuItem();
    void selectMenuItem();




    /* ==== DISPLAY DATA ===== */
    void setMasterData(const char* label, float value, const char* unit);
    void setSlaveData(const char* label, float value, const char* unit);
    void setSlaveText(const char* label, const char* text);




    /* ===== INPUT ===== */
    void onTouch();         // ESP32 Touch pin
    void onSwitch();        // TTP223
    void handleButton(uint8_t btn_pin);

    void setTemperature(float t);
    void setWiFiState(NetState state);



    /* ===== POWER / BRIGTHNESS ====== */
    void setBrightness(uint8_t level);
    void enableAutoBrightness(bool enable);
    void updateAmbientLight(uint16_t lightValue);
    void sleep();
    void wake();




    /* ====== DEBUG ====== */
    void enableDebug(bool enable);

private:

    /* ============ INTERNAL DRAW ============== */
    
    void drawBoot();
    void drawStatus();
    void drawMenu();
    void drawMasterData();
    void drawSlaveData();
    void drawSettings();
  

    void applyBrightness();

    /* ============ DISPLAY ==================== */
    Adafruit_SSD1306 display;
    Screen currentScreen = Screen::BOOT;



    /* ============ STATE ====================== */
    NetInfo netInfo;
    BrightnessControl brightness {255, true};
    bool sleeping = false;
    bool debug = false;



    /* ======================= MENU ==================== */
    static constexpr uint8_t MENU_ITEMS = 5;
    uint8_t menuIndex = 0;
    const char* menuLabels[MENU_ITEMS] = {
        "Status",
        "Master",
        "Slave",
        "Settings",
        "Sleep"
    };

    Screen menuScreens[MENU_ITEMS] = {
        Screen::STATUS,
        Screen::MASTER_DATA,
        Screen::SLAVE_DATA,
        Screen::SETTINGS,
        Screen::SLEEP
    };


    /* ========== DATA CACHE ========== */
    struct DataItem{
        const char* label;
        float value;
        const char* unit;
    };

    DataItem masterData[5];
    DataItem slaveData[5];

    uint8_t masterDataCount = 0;
    uint8_t slaveDataCount = 0;

    DisplayText slaveText[2];
    uint8_t slaveTextCount = 0;


    
    /* ========== TIMING ============= */
    uint32_t lastUpdate = 0;
    uint32_t animationTick = 0;
    uint8_t animFrame = 0;
    uint32_t lastInteraction = 0;
    static constexpr uint32_t UI_TIMEOUT_MS = 15000;
};  