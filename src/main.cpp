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
 * @author Veysel Karani Kılıçerkan
 * @date 25 Şubat 2026
 * @customer Umutcan Yılmaz
 */

#include <Arduino.h>
#include "ESP32_P4_Matrix.h"

// Global nesneler
ESP32_P4_Matrix* ledMatrix = nullptr;

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n================================");
    Serial.println("  Vakitmatik P4 LED Panel");
    Serial.println("  ESP32-S3 - 240x240");
    Serial.println("================================\n");
    
    // LED Matrix başlat
    Serial.println("Matrix başlatılıyor...");
    ledMatrix = new ESP32_P4_Matrix();
    
    if (ledMatrix->begin()) {
        Serial.println("✓ Matrix başarıyla başlatıldı!");
        ledMatrix->testPattern();
    } else {
        Serial.println("✗ Matrix başlatılamadı!");
        while(1) { delay(1000); }
    }
    
    Serial.println("\nSetup tamamlandı!");
}

void loop() {
    // Ana loop - şimdilik boş
    // Gelecekte burada:
    // - Saat güncelleme
    // - Vakıt kontrolü
    // - Animasyonlar
    
    delay(100);
}
