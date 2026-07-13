#include "OledDisplay.h"


#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK




// ----------------------------------------------------- ===== CONSTRUCTOR ===== ---------------------------------------------
OledDisplay::OledDisplay(uint8_t width, uint8_t height, uint8_t i2cAddress): display(width, height, &Wire, -1), currentScreen(Screen::BOOT){}





// --------------------------------------------------- ===== INITIALIZARE ===== -----------------------------------------------
bool OledDisplay::begin(TwoWire& wire){
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
        return false;
    }
    wolfMood = WolfMood::CONNECTING;

    netInfo.state = NetState::CONNECTING;
    netInfo.rssi = -100;
    netInfo.packetsRx = 0;
    netInfo.lastPacketMs = 0;



    display.clearDisplay();
    display.display();

    playBootAnimation();
    currentScreen = Screen::MENU;
    animationTick = millis();
    return true;
}

void OledDisplay::update(){
    if (sleeping) return;
    uint32_t now = millis();

    if(currentScreen != Screen::STATUS &&
        currentScreen != Screen::SLEEP &&
        (now - lastInteraction > UI_TIMEOUT_MS)){
            setScreen(Screen::STATUS);
            return;
        }
        
    

    switch (currentScreen)
    {
        case Screen::STATUS:        drawStatus(); break;
        case Screen::MENU:          drawMenu(); break;
        case Screen::MASTER_DATA:   drawMasterData(); break;
        case Screen::SLAVE_DATA:    drawSlaveData(); break;
        case Screen::SETTINGS:      drawSettings(); break;
        case Screen::SLEEP:         sleep(); break;
    default:
        break;
    }
}





// ----------------------------------------------------- ===== REFRESH ===== --------------------------------------------------
void OledDisplay::refresh(){
    display.clearDisplay();
    display.display();
}






// ----------------------------------------------------- ===== ANIMATII ===== --------------------------------------------------
void OledDisplay::playBootAnimation(){
    display.clearDisplay();
    display.setTextColor(WHITE);

    // 1. Logo text fade-in
    for (int y = 32; y >= 20; y--){
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(18, y);
        display.print("MASTER");
        display.display();
        delay(30);
    }
    delay(300);

    //2. Subtitlu
    display.setTextSize(1);
    display.setCursor(34, 42);
    display.print("intitializing");
    display.display();
    delay(300);

    //3. Progress bar
    for (int i = 0; i <= 100; i += 4){
        display.fillRect(14, 54, (i * 100) / 100, 6, SSD1306_WHITE);
        display.drawRect(14, 54, 100, 6, SSD1306_WHITE);
        display.display();
        delay(40);
    }
    delay(400);

    //4. Ready
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(28, 22);
    display.print("READY");
    display.display();

    delay(500);
}

void OledDisplay::drawNetStatus(){
    const uint8_t x = 108;
    const uint8_t y = 12;

     if (millis() - animationTick > 300){
        animFrame = (animFrame + 1) % 4;
        animationTick = millis();
    }

    if (netInfo.state == NetState::DISCONNECTED){
        display.drawLine(x, y, x + 12, y + 12, SSD1306_WHITE);
        display.drawLine(x + 12, y, x, y + 12, SSD1306_WHITE);
        return;
    }

    
    uint8_t bars = animFrame;
    if(netInfo.state == NetState::CONNECTED){
        bars = 3;
    }

    for(uint8_t i = 0; i < bars; i++){
        display.drawCircle(x, y, 3+i*3, WHITE);
    }
}






// -------------------------------------------------------- ===== SETTER ===== --------------------------------------------------
void OledDisplay::setNetInfo(const NetInfo& info){
    netInfo = info;
}

void OledDisplay::setScreen(Screen screen){
    currentScreen = screen;
    refresh();
}

OledDisplay::Screen OledDisplay::getCurrentScreen() const{
    return currentScreen;
}

void OledDisplay::setMasterData(const char* label, float value, const char* unit){
    for(uint8_t i = 0; i < masterDataCount; i++){
        if(strcmp(masterData[i].label, label) == 0){
            masterData[i].value = value;
            masterData[i].unit = unit;
            return;
        }
    }
    if(masterDataCount < 5){
        masterData[masterDataCount++] = {label, value, unit};
    }
}

void OledDisplay::setSlaveData(const char* label, float value, const char* unit){
    for(uint8_t i = 0; i < slaveDataCount; i++){
        if(strcmp(slaveData[i].label, label) == 0){
            slaveData[i].value = value;
            slaveData[i].unit = unit;
            return;
        }
    }
    if(slaveDataCount < 5){
        slaveData[slaveDataCount++] = {label, value, unit};
    }
}

void OledDisplay::setSlaveText(const char* label, const char* text){
    for (uint8_t i = 0; i < slaveTextCount; i++){
        if (strcmp(slaveText[i].label, label) == 0){
            slaveText[i].text = text;
            return;
        }
    }
    if (slaveTextCount < 2){
        slaveText[slaveTextCount++] = {label, text};
    }
}





// ----------------------------------------------------- ===== DRAW ===== --------------------------------------------------
void OledDisplay::drawStatus(){
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("~~~~ Status screen ~~~~");

    drawNetStatus();

    display.setCursor(0, 16);
    display.print("WiFi: ");

    switch (netInfo.state) {
        case NetState::CONNECTED:    display.print("CONNECTED"); break;
        case NetState::CONNECTING:   display.print("CONNECTING"); break;
        case NetState::DISCONNECTED: display.print("OFF"); break;
        case NetState::ERROR:        display.print("ERROR"); break;
    }

    display.setCursor(0, 26);
    display.print("RSSI: ");
    display.print(netInfo.rssi);
    display.print(" dBm");
    
    display.setCursor(0, 36);
    display.print("RX: ");
    display.print(netInfo.packetsRx);
    
    display.setCursor(0, 46);
    display.print("Last: ");
    display.print((millis() - netInfo.lastPacketMs) / 1000);
    display.print("s");
    display.display();
}

void OledDisplay::drawMenu(){
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);
    display.println("====== MENU ======");

    const uint8_t startY = 14;
    const uint8_t lineHeight = 10;


    for(uint8_t i = 0; i < MENU_ITEMS; i++){
        uint8_t y = startY + i * lineHeight;

        if ( i == menuIndex){
            // highlight bar
            display.fillRect(0, y-1, display.width(), lineHeight, WHITE);
            display.setTextColor(BLACK);

            // indicator
            if((millis() / 300) % 2 == 0){
                display.setCursor(2, y);
                display.print(">");
            }
        }else {
            display.setTextColor(WHITE);
        }
        display.setCursor(10,y);
        display.print(menuLabels[i]);
    }
    display.display();
}

void OledDisplay::drawMasterData(){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0, 0);
    display.println("==== MASTER DATA ====");

    uint8_t y = 12;
    for (uint8_t i = 0; i < masterDataCount; i++) {
    display.setCursor(0, y);
    display.print(masterData[i].label);
    display.print(": ");
    display.print(masterData[i].value, 1);
    display.print(" ");
    display.print(masterData[i].unit);
    y += 10;
    }

    display.display();
}

void OledDisplay::drawSlaveData(){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);
    display.print("==== SLAVE DATA ====");

    uint8_t y = 12;
    for (uint8_t i = 0; i < slaveDataCount; i++) {
    display.setCursor(0, y);
    display.print(slaveData[i].label);
    display.print(": ");
    display.print(slaveData[i].value, 1);
    display.print(" ");
    display.print(slaveData[i].unit);
    y += 10;
    }

    for (uint8_t i = 0; i < slaveTextCount; i++) {
        display.setCursor(0, y);
        display.print(slaveText[i].label);
        display.print(": ");
        display.print(slaveText[i].text);
        y += 10;
    }
    display.display();
}

void OledDisplay::drawSettings() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);
    display.println("SETTINGS");

    display.setCursor(0, 14);
    display.print("Brightness: ");
    display.println(brightness.level);

    display.setCursor(0, 24);
    display.print("Auto dim: ");
    display.println(brightness.autoMode ? "ON" : "OFF");

    display.display();
}





// ----------------------------------------------------- ===== SETTINGS ===== --------------------------------------------------
void OledDisplay::setBrightness(uint8_t level){
    brightness.level = level;
    applyBrightness();
}

void OledDisplay::applyBrightness(){
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(brightness.level);
}





// ----------------------------------------------------- ===== POWER ===== --------------------------------------------------
void OledDisplay::wake(){
    if(!sleeping) return;
    sleeping = false;
    display.ssd1306_command(SSD1306_DISPLAYON);
    lastInteraction = millis();
    setScreen(Screen::STATUS);
    //Serial.println("Wake up!!");
}

void OledDisplay::sleep(){
    if(!sleeping){
        sleeping = true;
        display.clearDisplay();
        display.display();
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        //Serial.println("Sleep");

    }
}




// ----------------------------------------------------- ===== FUNCTII MENIU ===== --------------------------------------------------
void OledDisplay::onTouch(){
    lastInteraction = millis();

    if (wolfMood == WolfMood::CONFUSED) {
        touchFailCounter++;
        if (touchFailCounter >= 3) {
            touchFailCounter = 0;
            wolfMood = WolfMood::CONNECTING;
            // aici vei pune retry WiFi
        }
        return;
    }

   nextMenuItem();
}

void OledDisplay::onSwitch(){
    selectMenuItem();       // touch lung
}

void OledDisplay::nextMenuItem(){
    lastInteraction = millis();
    if(currentScreen == Screen::MENU){
        menuIndex = (menuIndex + 1) % MENU_ITEMS;
    } else {
        setScreen(Screen::MENU);
    }
}

void OledDisplay::previousMenuItem(){
    if(currentScreen == Screen::MENU){
        menuIndex = (menuIndex == 0) ? MENU_ITEMS - 1 : menuIndex - 1;
    }
}

void OledDisplay::selectMenuItem(){
    lastInteraction = millis();
    if (currentScreen == Screen::MENU)
    {
        setScreen(menuScreens[menuIndex]);
    }else{
        setScreen(Screen::MENU);
    }
    
}

void OledDisplay::handleButton(uint8_t btn_pin){
    
    /* ---=== BUTTON HANDLING ===---- */
    static bool lastState = LOW;
    static uint32_t pressStar = 0;

    bool state = digitalRead(btn_pin);

    //Press
    if (state && !lastState){
        if(state && getCurrentScreen() == Screen::SLEEP){
            wake();
            lastState = state;
            update();
            return;
        }
        pressStar = millis();
    }


    //Release
    if(!state && lastState){
        uint32_t duration = millis() - pressStar;

        if(duration >= LONG_PRESS_MS){
            selectMenuItem();
        }else {
            nextMenuItem();
        }
    }

    lastState = state;
}


