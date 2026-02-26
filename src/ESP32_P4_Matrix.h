/**
 * ESP32_P4_Matrix.h
 * 
 * ESP32-S3 için 240x240 P4 LED Panel sürücüsü
 * HUB75 DMA kullanarak 18 panel kontrolü
 * 
 * Özellikler:
 * - 3x6 panel dizilimi (240x240 toplam)
 * - 1/10 Scan Folded Matrix mapping
 * - PSRAM frame buffer
 * - Türkçe karakter desteği
 * 
 * @author Veysel Karani Kılıçerkan
 */

#ifndef ESP32_P4_MATRIX_H
#define ESP32_P4_MATRIX_H

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

// Panel konfigürasyonu (platformio.ini'den gelecek)
#ifndef PANEL_WIDTH
    #define PANEL_WIDTH 80
#endif

#ifndef PANEL_HEIGHT
    #define PANEL_HEIGHT 40
#endif

#ifndef PANELS_X
    #define PANELS_X 3
#endif

#ifndef PANELS_Y
    #define PANELS_Y 6
#endif

#define TOTAL_WIDTH (PANEL_WIDTH * PANELS_X)   // 240
#define TOTAL_HEIGHT (PANEL_HEIGHT * PANELS_Y) // 240

// Renk tanımlamaları (RGB565 format)
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F
#define COLOR_ORANGE    0xFD20

// Namaz vakitleri yapısı
struct PrayerTimes {
    String fajr;     // İmsak
    String sunrise;  // Güneş
    String dhuhr;    // Öğle
    String asr;      // İkindi
    String maghrib;  // Akşam
    String isha;     // Yatsı
};

class ESP32_P4_Matrix {
public:
    ESP32_P4_Matrix();
    ~ESP32_P4_Matrix();
    
    /**
     * Matrix'i başlat
     * @return true başarılı, false hata
     */
    bool begin();
    
    /**
     * Ekranı temizle
     * @param color Arkaplan rengi (varsayılan: siyah)
     */
    void clear(uint16_t color = COLOR_BLACK);
    
    /**
     * Test pattern göster (grid)
     */
    void testPattern();
    
    /**
     * Parlaklık ayarla
     * @param brightness 0-255 arası değer
     */
    void setBrightness(uint8_t brightness);
    
    /**
     * Pixel çiz
     * @param x X koordinatı (0-239)
     * @param y Y koordinatı (0-239)
     * @param color RGB565 renk
     */
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    
    /**
     * Çizgi çiz
     */
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    
    /**
     * Dikdörtgen çiz
     */
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    
    /**
     * Dolu dikdörtgen çiz
     */
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    
    /**
     * Metin yaz (basit, Türkçe desteği için TurkishFont kullanılacak)
     */
    void drawText(const char* text, int16_t x, int16_t y, uint16_t color);
    
    /**
     * Türkçe karakterli metin yaz
     * @param text UTF-8 formatında metin
     * @param x X koordinatı
     * @param y Y koordinatı
     * @param color RGB565 renk
     * @param size Yazı boyutu (1-3)
     */
    void drawTextTurkish(const String& text, int16_t x, int16_t y, uint16_t color, uint8_t size = 1);
    
    /**
     * WiFi'a bağlan
     * @param ssid WiFi ağ adı
     * @param password WiFi şifresi
     * @param timeout Timeout süresi (ms)
     * @return true başarılı, false hata
     */
    bool connectWiFi(const char* ssid, const char* password, uint32_t timeout = 10000);
    
    /**
     * NTP sunucusundan saat al
     * @param ntpServer NTP sunucu adresi (varsayılan: tr.pool.ntp.org)
     * @param gmtOffset GMT offset (Türkiye için 3*3600)
     * @return true başarılı, false hata
     */
    bool syncTime(const char* ntpServer = "tr.pool.ntp.org", long gmtOffset = 10800);
    
    /**
     * Şu anki saati al
     * @return HH:MM:SS formatında saat
     */
    String getCurrentTime();
    
    /**
     * Şu anki tarihi al
     * @return DD.MM.YYYY formatında tarih
     */
    String getCurrentDate();
    
    /**
     * Namaz vakitlerini ayarla (manuel)
     * @param times PrayerTimes yapısı
     */
    void setPrayerTimes(const PrayerTimes& times);
    
    /**
     * Namaz vakitlerini göster
     * @param x X koordinatı
     * @param y Y koordinatı
     */
    void displayPrayerTimes(int16_t x = 0, int16_t y = 0);
    
    /**
     * Saat göster (büyük)
     * @param x X koordinatı
     * @param y Y koordinatı
     * @param color RGB565 renk
     */
    void displayClock(int16_t x = 0, int16_t y = 0, uint16_t color = COLOR_WHITE);
    
    /**
     * DMA display nesnesine erişim (ileri seviye kullanım)
     */
    MatrixPanel_I2S_DMA* getDisplay() { return dma_display; }
    
    /**
     * WiFi bağlı mı?
     */
    bool isWiFiConnected() { return WiFi.status() == WL_CONNECTED; }
    
private:
    MatrixPanel_I2S_DMA* dma_display;
    PrayerTimes prayerTimes;
    WiFiUDP ntpUDP;
    NTPClient* timeClient;
    bool timeInitialized;
    
    /**
     * HUB75 pinlerini ayarla
     */
    void setupPins(HUB75_I2S_CFG::i2s_pins& pins);
    
    /**
     * 1/10 Scan mapping (şimdilik basit, TASK-004'te geliştirilecek)
     */
    void mapCoordinates(int16_t& x, int16_t& y);
    
    /**
     * Türkçe karakter dönüşümü (UTF-8 -> ASCII yaklaşık)
     */
    String convertTurkishChars(const String& text);
    
    /**
     * Büyük rakam çiz (7-segment tarzı)
     */
    void drawBigDigit(char digit, int16_t x, int16_t y, uint16_t color, uint8_t scale = 1);
};

#endif // ESP32_P4_MATRIX_H
