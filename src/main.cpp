/**
 * Vakitmatik P4 LED Panel - Ana Program
 * 
 * ESP32-S3 tabanlı 240x240 LED panel vakıt gösterimi
 * 
 * Donanım:
 * - ESP32-S3 N16R8 (16MB Flash, 8MB PSRAM)
 * - 18 adet P4 LED Panel (80x40, 1/10 scan)
 * - ICN2037BP Sürücü
 * 
 * Özellikler:
 * - WiFi bağlantısı ve NTP saat senkronizasyonu
 * - Namaz vakitlerini gösterme
 * - Türkçe karakter desteği
 * - Büyük dijital saat gösterimi
 * 
 * @author Veysel Karani Kılıçerkan
 * @date 26 Şubat 2026
 * @customer Umutcan Yılmaz
 */

#include <Arduino.h>
#include "ESP32_P4_Matrix.h"

// WiFi Ayarları (MÜŞTERİ DÜZENLEYECEK)
const char* WIFI_SSID = "WiFi_Adi";        // Buraya WiFi adını yaz
const char* WIFI_PASSWORD = "WiFi_Sifresi"; // Buraya WiFi şifresini yaz

// Global nesneler
ESP32_P4_Matrix* ledMatrix = nullptr;

// Çalışma modları
enum DisplayMode {
    MODE_CLOCK,         // Dijital saat gösterimi
    MODE_PRAYER_TIMES,  // Namaz vakitleri
    MODE_TEST           // Test yaz
};

DisplayMode currentMode = MODE_CLOCK;
unsigned long lastUpdate = 0;
unsigned long modeChangeInterval = 10000; // Her 10 saniyede mod değiştir

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n================================");
    Serial.println("  Vakitmatik P4 LED Panel");
    Serial.println("  ESP32-S3 - 240x240");
    Serial.println("  v1.0 - Full Featured");
    Serial.println("================================\n");
    
    // LED Matrix başlat
    Serial.println("Matrix başlatılıyor...");
    ledMatrix = new ESP32_P4_Matrix();
    
    if (!ledMatrix->begin()) {
        Serial.println("✗ Matrix başlatılamadı!");
        while(1) { delay(1000); }
    }
    
    Serial.println("✓ Matrix başarıyla başlatıldı!");
    
    // İlk test
    Serial.println("\nHızlı test başlatılıyor...");
    ledMatrix->testPattern();
    
    // WiFi bağlantısı
    Serial.println("\nWiFi bağlantısı kuruluyor...");
    if (ledMatrix->connectWiFi(WIFI_SSID, WIFI_PASSWORD, 15000)) {
        // NTP senkronizasyonu
        if (ledMatrix->syncTime()) {
            Serial.println("✓ Sistem hazır!");
        } else {
            Serial.println("⚠ NTP senkronizasyonu başarısız, varsayılan saat kullanılacak");
        }
    } else {
        Serial.println("⚠ WiFi bağlantısı başarısız, offline modda çalışacak");
    }
    
    // Namaz vakitlerini ayarla (örnek - müşteri kendi vakitlerini girecek)
    PrayerTimes times;
    times.fajr = "05:45";
    times.sunrise = "07:10";
    times.dhuhr = "12:35";
    times.asr = "15:50";
    times.maghrib = "18:20";
    times.isha = "19:50";
    ledMatrix->setPrayerTimes(times);
    
    Serial.println("\nSetup tamamlandı!");
    Serial.println("Ana döngü başlıyor...\n");
    
    lastUpdate = millis();
}

void loop() {
    unsigned long currentTime = millis();
    
    // Her 10 saniyede mod değiştir
    if (currentTime - lastUpdate > modeChangeInterval) {
        lastUpdate = currentTime;
        
        // Modu değiştir
        switch (currentMode) {
            case MODE_CLOCK:
                currentMode = MODE_PRAYER_TIMES;
                Serial.println("→ Mod: Namaz Vakitleri");
                ledMatrix->displayPrayerTimes(10, 10);
                break;
                
            case MODE_PRAYER_TIMES:
                currentMode = MODE_CLOCK;
                Serial.println("→ Mod: Dijital Saat");
                ledMatrix->clear();
                break;
                
            case MODE_TEST:
                currentMode = MODE_CLOCK;
                break;
        }
    }
    
    // Saat modunda sürekli güncelle
    if (currentMode == MODE_CLOCK) {
        ledMatrix->clear(COLOR_BLACK);
        ledMatrix->displayClock(20, 80, COLOR_GREEN);
        
        // WiFi durumu göster
        if (ledMatrix->isWiFiConnected()) {
            ledMatrix->drawText("WiFi: OK", 10, 220, COLOR_GREEN);
        } else {
            ledMatrix->drawText("WiFi: OFF", 10, 220, COLOR_RED);
        }
        
        delay(1000); // Her saniye güncelle
    }
    
    delay(100);
}
