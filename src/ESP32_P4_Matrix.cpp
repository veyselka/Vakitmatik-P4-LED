/**
 * ESP32_P4_Matrix.cpp
 * 
 * ESP32-S3 P4 LED Panel sürücüsü implementasyonu
 * 
 * @author Veysel Karani Kılıçerkan
 */

#include "ESP32_P4_Matrix.h"

ESP32_P4_Matrix::ESP32_P4_Matrix() {
    dma_display = nullptr;
    timeClient = nullptr;
    timeInitialized = false;
    
    // Varsayılan namaz vakitleri (örnek)
    prayerTimes.fajr = "05:30";
    prayerTimes.sunrise = "07:00";
    prayerTimes.dhuhr = "12:30";
    prayerTimes.asr = "15:45";
    prayerTimes.maghrib = "18:15";
    prayerTimes.isha = "19:45";
}

ESP32_P4_Matrix::~ESP32_P4_Matrix() {
    if (dma_display != nullptr) {
        delete dma_display;
    }
    if (timeClient != nullptr) {
        delete timeClient;
    }
}

bool ESP32_P4_Matrix::begin() {
    Serial.println("Initializing HUB75 DMA...");
    
    // Pin konfigürasyonu
    HUB75_I2S_CFG::i2s_pins pins;
    setupPins(pins);
    
    // HUB75 konfigürasyonu
    HUB75_I2S_CFG mxconfig(
        PANEL_WIDTH,      // Panel genişliği (80)
        PANEL_HEIGHT,     // Panel yüksekliği (40)
        PANELS_X * PANELS_Y,  // Toplam panel sayısı (18)
        pins              // Pin mapping
    );
    
    // 1/10 Scan için özel ayarlar
    mxconfig.clkphase = false;
    mxconfig.driver = HUB75_I2S_CFG::ICN2038S; // ICN2037BP benzeri
    mxconfig.latch_blanking = 4;
    mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;
    
    // DMA display oluştur
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    
    if (dma_display == nullptr) {
        Serial.println("ERROR: DMA display allocation failed!");
        return false;
    }
    
    // Başlat
    if (!dma_display->begin()) {
        Serial.println("ERROR: DMA display begin() failed!");
        delete dma_display;
        dma_display = nullptr;
        return false;
    }
    
    // İlk kurulum
    dma_display->setBrightness8(128); // %50 parlaklık
    dma_display->clearScreen();
    
    Serial.printf("✓ Matrix initialized: %dx%d pixels\n", TOTAL_WIDTH, TOTAL_HEIGHT);
    Serial.printf("  Panels: %d (%dx%d layout)\n", PANELS_X * PANELS_Y, PANELS_X, PANELS_Y);
    Serial.printf("  PSRAM: %s\n", psramFound() ? "Available" : "NOT FOUND!");
    
    return true;
}

void ESP32_P4_Matrix::setupPins(HUB75_I2S_CFG::i2s_pins& pins) {
    // platformio.ini'den gelen tanımlamalar
    #ifdef R1_PIN
        pins.r1 = R1_PIN;
        pins.g1 = G1_PIN;
        pins.b1 = B1_PIN;
        pins.r2 = R2_PIN;
        pins.g2 = G2_PIN;
        pins.b2 = B2_PIN;
        pins.a = A_PIN;
        pins.b = B_PIN;
        pins.c = C_PIN;
        pins.d = D_PIN;
        pins.e = E_PIN;
        pins.lat = LAT_PIN;
        pins.oe = OE_PIN;
        pins.clk = CLK_PIN;
    #else
        // Varsayılan pinler (platformio.ini yoksa)
        pins.r1 = 25;
        pins.g1 = 26;
        pins.b1 = 27;
        pins.r2 = 14;
        pins.g2 = 12;
        pins.b2 = 13;
        pins.a = 23;
        pins.b = 22;
        pins.c = 5;
        pins.d = 17;
        pins.e = -1;  // 1/10 scan, E kullanılabilir
        pins.lat = 4;
        pins.oe = 15;
        pins.clk = 16;
    #endif
    
    Serial.println("Pin Configuration:");
    Serial.printf("  R1=%d G1=%d B1=%d\n", pins.r1, pins.g1, pins.b1);
    Serial.printf("  R2=%d G2=%d B2=%d\n", pins.r2, pins.g2, pins.b2);
    Serial.printf("  A=%d B=%d C=%d D=%d E=%d\n", pins.a, pins.b, pins.c, pins.d, pins.e);
    Serial.printf("  LAT=%d OE=%d CLK=%d\n", pins.lat, pins.oe, pins.clk);
}

void ESP32_P4_Matrix::clear(uint16_t color) {
    if (dma_display == nullptr) return;
    
    if (color == COLOR_BLACK) {
        dma_display->clearScreen();
    } else {
        dma_display->fillScreenRGB888(
            (color >> 11) << 3,  // R
            ((color >> 5) & 0x3F) << 2,  // G
            (color & 0x1F) << 3   // B
        );
    }
}

void ESP32_P4_Matrix::testPattern() {
    if (dma_display == nullptr) return;
    
    Serial.println("Drawing test pattern...");
    
    // Ekranı temizle
    clear(COLOR_BLACK);
    delay(500);
    
    // 1. Test: RGB renkler (tüm ekran)
    Serial.println("  Test 1: RGB Colors");
    clear(COLOR_RED);
    delay(1000);
    clear(COLOR_GREEN);
    delay(1000);
    clear(COLOR_BLUE);
    delay(1000);
    clear(COLOR_BLACK);
    
    // 2. Test: Çerçeve
    Serial.println("  Test 2: Frame");
    drawRect(0, 0, TOTAL_WIDTH, TOTAL_HEIGHT, COLOR_WHITE);
    delay(1000);
    
    // 3. Test: Grid (her 20 pixelde bir çizgi)
    Serial.println("  Test 3: Grid Pattern");
    clear(COLOR_BLACK);
    
    // Dikey çizgiler
    for (int x = 0; x < TOTAL_WIDTH; x += 20) {
        drawLine(x, 0, x, TOTAL_HEIGHT - 1, COLOR_GREEN);
    }
    
    // Yatay çizgiler
    for (int y = 0; y < TOTAL_HEIGHT; y += 20) {
        drawLine(0, y, TOTAL_WIDTH - 1, y, COLOR_GREEN);
    }
    
    delay(2000);
    
    // 4. Test: Köşe pikselleri
    Serial.println("  Test 4: Corner Pixels");
    clear(COLOR_BLACK);
    drawPixel(0, 0, COLOR_RED);                             // Sol üst
    drawPixel(TOTAL_WIDTH - 1, 0, COLOR_GREEN);              // Sağ üst
    drawPixel(0, TOTAL_HEIGHT - 1, COLOR_BLUE);              // Sol alt
    drawPixel(TOTAL_WIDTH - 1, TOTAL_HEIGHT - 1, COLOR_YELLOW); // Sağ alt
    
    delay(2000);
    
    // 5. Test: Merkez çizgi
    Serial.println("  Test 5: Center Lines");
    drawLine(TOTAL_WIDTH / 2, 0, TOTAL_WIDTH / 2, TOTAL_HEIGHT - 1, COLOR_CYAN);
    drawLine(0, TOTAL_HEIGHT / 2, TOTAL_WIDTH - 1, TOTAL_HEIGHT / 2, COLOR_MAGENTA);
    
    delay(2000);
    
    Serial.println("✓ Test pattern complete!");
    clear(COLOR_BLACK);
}

void ESP32_P4_Matrix::setBrightness(uint8_t brightness) {
    if (dma_display == nullptr) return;
    dma_display->setBrightness8(brightness);
}

void ESP32_P4_Matrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (dma_display == nullptr) return;
    
    // Sınır kontrolü
    if (x < 0 || x >= TOTAL_WIDTH || y < 0 || y >= TOTAL_HEIGHT) return;
    
    // TODO: TASK-004'te 1/10 scan mapping eklenecek
    // mapCoordinates(x, y);
    
    dma_display->drawPixel(x, y, color);
}

void ESP32_P4_Matrix::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    if (dma_display == nullptr) return;
    dma_display->drawLine(x0, y0, x1, y1, color);
}

void ESP32_P4_Matrix::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (dma_display == nullptr) return;
    dma_display->drawRect(x, y, w, h, color);
}

void ESP32_P4_Matrix::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (dma_display == nullptr) return;
    dma_display->fillRect(x, y, w, h, color);
}

void ESP32_P4_Matrix::drawText(const char* text, int16_t x, int16_t y, uint16_t color) {
    if (dma_display == nullptr) return;
    
    // Basit metin (Adafruit GFX varsayılan fontu)
    // TODO: TASK-006'da Türkçe font eklenecek
    dma_display->setTextColor(color);
    dma_display->setCursor(x, y);
    dma_display->print(text);
}

void ESP32_P4_Matrix::mapCoordinates(int16_t& x, int16_t& y) {
    // TODO: TASK-004 - 1/10 Scan Folded Matrix mapping
    // Şimdilik direkt geçiş yapıyor (basit mapping)
    
    // Gelecekte burada:
    // - Panel numarasını bul (0-17)
    // - Local koordinatları hesapla
    // - 1/10 scan için Y koordinatını dönüştür
    // - Folded matrix algoritması uygula
}
// ============================================================================
// WiFi ve NTP Fonksiyonları
// ============================================================================

bool ESP32_P4_Matrix::connectWiFi(const char* ssid, const char* password, uint32_t timeout) {
    Serial.printf("WiFi'a bağlanılıyor: %s\n", ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > timeout) {
            Serial.println("✗ WiFi bağlantısı zaman aşımı!");
            return false;
        }
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\n✓ WiFi bağlandı!");
    Serial.printf("  IP Adresi: %s\n", WiFi.localIP().toString().c_str());
    return true;
}

bool ESP32_P4_Matrix::syncTime(const char* ntpServer, long gmtOffset) {
    Serial.printf("NTP senkronizasyonu: %s\n", ntpServer);
    
    if (!isWiFiConnected()) {
        Serial.println("✗ WiFi bağlı değil!");
        return false;
    }
    
    // NTPClient başlat
    if (timeClient == nullptr) {
        timeClient = new NTPClient(ntpUDP, ntpServer, gmtOffset, 60000);
    }
    
    timeClient->begin();
    
    // İlk güncelleme
    if (!timeClient->update()) {
        Serial.println("✗ NTP güncellemesi başarısız!");
        return false;
    }
    
    timeInitialized = true;
    Serial.println("✓ NTP senkronizasyonu başarılı!");
    Serial.printf("  Saat: %s\n", getCurrentTime().c_str());
    
    return true;
}

String ESP32_P4_Matrix::getCurrentTime() {
    if (!timeInitialized || timeClient == nullptr) {
        return "--:--:--";
    }
    
    timeClient->update();
    return timeClient->getFormattedTime();
}

String ESP32_P4_Matrix::getCurrentDate() {
    if (!timeInitialized) {
        return "--.--.----";
    }
    
    time_t rawtime = timeClient->getEpochTime();
    struct tm* timeinfo = localtime(&rawtime);
    
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d.%02d.%04d", 
             timeinfo->tm_mday, 
             timeinfo->tm_mon + 1, 
             timeinfo->tm_year + 1900);
    
    return String(buffer);
}

// ============================================================================
// Namaz Vakitleri Fonksiyonları
// ============================================================================

void ESP32_P4_Matrix::setPrayerTimes(const PrayerTimes& times) {
    prayerTimes = times;
    Serial.println("Namaz vakitleri güncellendi:");
    Serial.printf("  İmsak  : %s\n", prayerTimes.fajr.c_str());
    Serial.printf("  Güneş  : %s\n", prayerTimes.sunrise.c_str());
    Serial.printf("  Öğle   : %s\n", prayerTimes.dhuhr.c_str());
    Serial.printf("  İkindi : %s\n", prayerTimes.asr.c_str());
    Serial.printf("  Akşam  : %s\n", prayerTimes.maghrib.c_str());
    Serial.printf("  Yatsı  : %s\n", prayerTimes.isha.c_str());
}

void ESP32_P4_Matrix::displayPrayerTimes(int16_t x, int16_t y) {
    if (dma_display == nullptr) return;
    
    clear(COLOR_BLACK);
    
    // Başlık
    drawTextTurkish("NAMAZ VAKITLERI", x + 10, y + 5, COLOR_GREEN, 2);
    
    // Vakitleri listele
    int16_t lineY = y + 30;
    int16_t lineSpacing = 30;
    
    drawTextTurkish("Imsak  : " + prayerTimes.fajr, x + 10, lineY, COLOR_CYAN, 1);
    lineY += lineSpacing;
    
    drawTextTurkish("Gunes  : " + prayerTimes.sunrise, x + 10, lineY, COLOR_YELLOW, 1);
    lineY += lineSpacing;
    
    drawTextTurkish("Ogle   : " + prayerTimes.dhuhr, x + 10, lineY, COLOR_WHITE, 1);
    lineY += lineSpacing;
    
    drawTextTurkish("Ikindi : " + prayerTimes.asr, x + 10, lineY, COLOR_ORANGE, 1);
    lineY += lineSpacing;
    
    drawTextTurkish("Aksam  : " + prayerTimes.maghrib, x + 10, lineY, COLOR_MAGENTA, 1);
    lineY += lineSpacing;
    
    drawTextTurkish("Yatsi  : " + prayerTimes.isha, x + 10, lineY, COLOR_BLUE, 1);
}

void ESP32_P4_Matrix::displayClock(int16_t x, int16_t y, uint16_t color) {
    if (dma_display == nullptr) return;
    
    String time = getCurrentTime();
    
    // Büyük rakamlarla saat göster
    // HH:MM formatı (saniye gösterme)
    String hourMin = time.substring(0, 5);
    
    drawTextTurkish(hourMin, x, y, color, 3);
    
    // Tarihi küçük göster
    String date = getCurrentDate();
    drawTextTurkish(date, x + 10, y + 50, COLOR_CYAN, 1);
}

// ============================================================================
// Türkçe Destek Fonksiyonları
// ============================================================================

String ESP32_P4_Matrix::convertTurkishChars(const String& text) {
    String result = text;
    
    // UTF-8 Türkçe karakterleri ASCII yaklaşımlarına çevir
    result.replace("ş", "s");
    result.replace("Ş", "S");
    result.replace("ğ", "g");
    result.replace("Ğ", "G");
    result.replace("ı", "i");
    result.replace("İ", "I");
    result.replace("ö", "o");
    result.replace("Ö", "O");
    result.replace("ü", "u");
    result.replace("Ü", "U");
    result.replace("ç", "c");
    result.replace("Ç", "C");
    
    return result;
}

void ESP32_P4_Matrix::drawTextTurkish(const String& text, int16_t x, int16_t y, uint16_t color, uint8_t size) {
    if (dma_display == nullptr) return;
    
    // Türkçe karakterleri dönüştür
    String converted = convertTurkishChars(text);
    
    dma_display->setTextColor(color);
    dma_display->setTextSize(size);
    dma_display->setCursor(x, y);
    dma_display->print(converted);
}

void ESP32_P4_Matrix::drawBigDigit(char digit, int16_t x, int16_t y, uint16_t color, uint8_t scale) {
    if (dma_display == nullptr) return;
    
    // 7-segment tarzı büyük rakam çizimi
    // Bu basit bir implementasyon, ileride geliştirilebilir
    
    dma_display->setTextColor(color);
    dma_display->setTextSize(scale * 4);
    dma_display->setCursor(x, y);
    dma_display->print(digit);
}