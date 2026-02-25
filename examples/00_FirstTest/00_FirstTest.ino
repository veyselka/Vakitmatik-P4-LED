/**
 * 00_FirstTest - İlk Test Programı
 * 
 * Bu program müşterinin donanımı aldığında ilk yükleyeceği koddur.
 * Amacı: Tüm panellerin çalıştığını doğrulamak
 * 
 * Test Senaryosu:
 * 1. Tüm ekran kırmızı yanar (2 saniye)
 * 2. Tüm ekran yeşil yanar (2 saniye)
 * 3. Tüm ekran mavi yanar (2 saniye)
 * 4. Grid pattern gösterir (10 saniye)
 * 5. Köşe pikselleri yanar (5 saniye)
 * 
 * BU TESTİ GEÇERLİ SONUÇLAR:
 * ✅ Tüm 18 panel ışık veriyor
 * ✅ Renkler doğru (kırmızı=kırmızı, yeşil=yeşil, mavi=mavi)
 * ✅ Grid çizgileri düz (bükülmüyor)
 * ✅ Köşe pikselleri doğru yerde
 * 
 * @author Veysel Karani Kılıçerkan
 * @customer Umutcan Yılmaz
 */

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Panel Konfigürasyonu
#define PANEL_WIDTH 80
#define PANEL_HEIGHT 40
#define PANELS_X 3      // Yatayda 3 panel
#define PANELS_Y 6      // Dikeyde 6 panel
#define TOTAL_WIDTH (PANEL_WIDTH * PANELS_X)   // 240
#define TOTAL_HEIGHT (PANEL_HEIGHT * PANELS_Y) // 240

// HUB75 Pin Tanımlamaları
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13
#define A_PIN 23
#define B_PIN 22
#define C_PIN 5
#define D_PIN 17
#define E_PIN -1
#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 16

MatrixPanel_I2S_DMA *dma_display = nullptr;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n========================================");
    Serial.println("  VAKITMATIK P4 LED PANEL - İLK TEST");
    Serial.println("========================================\n");
    
    // PSRAM kontrolü
    if (!psramFound()) {
        Serial.println("HATA: PSRAM BULUNAMADI!");
        Serial.println("ESP32-S3 N16R8 (8MB PSRAM) gerekli!");
        while(1) {
            delay(1000);
            Serial.println("PSRAM YOK - DURDURULDU");
        }
    }
    Serial.println("✓ PSRAM bulundu");
    
    // Pin yapılandırması
    HUB75_I2S_CFG::i2s_pins _pins = {
        R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
        A_PIN, B_PIN, C_PIN, D_PIN, E_PIN,
        LAT_PIN, OE_PIN, CLK_PIN
    };
    
    Serial.println("Pin Konfigürasyonu:");
    Serial.printf("  R1=%d G1=%d B1=%d\n", R1_PIN, G1_PIN, B1_PIN);
    Serial.printf("  R2=%d G2=%d B2=%d\n", R2_PIN, G2_PIN, B2_PIN);
    Serial.printf("  A=%d B=%d C=%d D=%d E=%d\n", A_PIN, B_PIN, C_PIN, D_PIN, E_PIN);
    
    // HUB75 konfigürasyonu
    HUB75_I2S_CFG mxconfig(
        PANEL_WIDTH,
        PANEL_HEIGHT,
        PANELS_X * PANELS_Y,
        _pins
    );
    
    // 1/10 Scan özel ayarlar
    mxconfig.clkphase = false;
    mxconfig.driver = HUB75_I2S_CFG::ICN2038S;
    mxconfig.latch_blanking = 4;
    
    Serial.println("\nMatrix başlatılıyor...");
    
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    
    if (dma_display == nullptr) {
        Serial.println("HATA: DMA display oluşturulamadı!");
        while(1) { delay(1000); }
    }
    
    if (!dma_display->begin()) {
        Serial.println("HATA: DMA display başlatılamadı!");
        while(1) { delay(1000); }
    }
    
    dma_display->setBrightness8(128); // %50 parlaklık
    dma_display->clearScreen();
    
    Serial.println("✓ Matrix başarıyla başlatıldı!");
    Serial.printf("  Çözünürlük: %dx%d piksel\n", TOTAL_WIDTH, TOTAL_HEIGHT);
    Serial.printf("  Toplam Panel: %d (%dx%d)\n", PANELS_X * PANELS_Y, PANELS_X, PANELS_Y);
    Serial.println("\n========================================");
    Serial.println("  TESTLER BAŞLIYOR...");
    Serial.println("========================================\n");
    
    delay(2000);
    
    // TEST 1: RGB Renk Testi
    Serial.println("TEST 1/5: RGB Renk Testi");
    Serial.println("  → Tüm ekran KIRMIZI olmalı");
    dma_display->fillScreenRGB888(255, 0, 0);
    delay(2000);
    
    Serial.println("  → Tüm ekran YEŞİL olmalı");
    dma_display->fillScreenRGB888(0, 255, 0);
    delay(2000);
    
    Serial.println("  → Tüm ekran MAVİ olmalı");
    dma_display->fillScreenRGB888(0, 0, 255);
    delay(2000);
    
    Serial.println("  → Tüm ekran BEYAZ olmalı");
    dma_display->fillScreenRGB888(255, 255, 255);
    delay(2000);
    
    dma_display->clearScreen();
    Serial.println("✓ Renk testi tamamlandı\n");
    
    delay(1000);
    
    // TEST 2: Grid Pattern
    Serial.println("TEST 2/5: Grid Pattern");
    Serial.println("  → 20 piksel aralıklı grid çiziliyor");
    
    // Dikey çizgiler (yeşil)
    for (int x = 0; x < TOTAL_WIDTH; x += 20) {
        for (int y = 0; y < TOTAL_HEIGHT; y++) {
            dma_display->drawPixelRGB888(x, y, 0, 255, 0);
        }
    }
    
    // Yatay çizgiler (mavi)
    for (int y = 0; y < TOTAL_HEIGHT; y += 20) {
        for (int x = 0; x < TOTAL_WIDTH; x++) {
            dma_display->drawPixelRGB888(x, y, 0, 0, 255);
        }
    }
    
    delay(5000);
    dma_display->clearScreen();
    Serial.println("✓ Grid testi tamamlandı\n");
    
    delay(1000);
    
    // TEST 3: Köşe Pikselleri
    Serial.println("TEST 3/5: Köşe Pikselleri");
    Serial.println("  → 4 köşede renkli piksel olmalı:");
    Serial.println("    Sol Üst: Kırmızı (0,0)");
    Serial.println("    Sağ Üst: Yeşil (239,0)");
    Serial.println("    Sol Alt: Mavi (0,239)");
    Serial.println("    Sağ Alt: Sarı (239,239)");
    
    dma_display->drawPixelRGB888(0, 0, 255, 0, 0);                       // Sol üst: Kırmızı
    dma_display->drawPixelRGB888(TOTAL_WIDTH-1, 0, 0, 255, 0);          // Sağ üst: Yeşil
    dma_display->drawPixelRGB888(0, TOTAL_HEIGHT-1, 0, 0, 255);         // Sol alt: Mavi
    dma_display->drawPixelRGB888(TOTAL_WIDTH-1, TOTAL_HEIGHT-1, 255, 255, 0); // Sağ alt: Sarı
    
    delay(5000);
    dma_display->clearScreen();
    Serial.println("✓ Köşe testi tamamlandı\n");
    
    delay(1000);
    
    // TEST 4: Merkez Çizgileri
    Serial.println("TEST 4/5: Merkez Çizgileri");
    Serial.println("  → Dikey ve yatay merkez çizgileri");
    
    // Dikey merkez (x=120)
    for (int y = 0; y < TOTAL_HEIGHT; y++) {
        dma_display->drawPixelRGB888(TOTAL_WIDTH/2, y, 0, 255, 255);
    }
    
    // Yatay merkez (y=120)
    for (int x = 0; x < TOTAL_WIDTH; x++) {
        dma_display->drawPixelRGB888(x, TOTAL_HEIGHT/2, 255, 0, 255);
    }
    
    delay(5000);
    dma_display->clearScreen();
    Serial.println("✓ Merkez testi tamamlandı\n");
    
    delay(1000);
    
    // TEST 5: Panel Sınırları
    Serial.println("TEST 5/5: Panel Sınırları");
    Serial.println("  → Her panelin etrafına çerçeve çiziliyor");
    
    for (int py = 0; py < PANELS_Y; py++) {
        for (int px = 0; px < PANELS_X; px++) {
            int x0 = px * PANEL_WIDTH;
            int y0 = py * PANEL_HEIGHT;
            
            // Çerçeve çiz (beyaz)
            for (int i = 0; i < PANEL_WIDTH; i++) {
                dma_display->drawPixelRGB888(x0 + i, y0, 255, 255, 255);
                dma_display->drawPixelRGB888(x0 + i, y0 + PANEL_HEIGHT - 1, 255, 255, 255);
            }
            for (int i = 0; i < PANEL_HEIGHT; i++) {
                dma_display->drawPixelRGB888(x0, y0 + i, 255, 255, 255);
                dma_display->drawPixelRGB888(x0 + PANEL_WIDTH - 1, y0 + i, 255, 255, 255);
            }
        }
    }
    
    delay(5000);
    dma_display->clearScreen();
    Serial.println("✓ Panel sınır testi tamamlandı\n");
    
    // Final Raporu
    Serial.println("\n========================================");
    Serial.println("  TÜM TESTLER TAMAMLANDI!");
    Serial.println("========================================\n");
    Serial.println("KONTROL LİSTESİ:");
    Serial.println("[ ] Tüm 18 panel ışık verdi mi?");
    Serial.println("[ ] Renkler doğru mu? (R=kırmızı, G=yeşil, B=mavi)");
    Serial.println("[ ] Grid çizgileri düz mü? (bükülme yok mu?)");
    Serial.println("[ ] Köşe pikselleri doğru yerde mi?");
    Serial.println("[ ] Panel sınırları belli mi?");
    Serial.println("[ ] Titreşim (flickering) var mı?");
    Serial.println("\nSONRAKİ ADIM:");
    Serial.println("➜ Bu testlerin FOTOĞRAFLARINI çek");
    Serial.println("➜ Veysel'e gönder");
    Serial.println("➜ Sorunlar varsa detaylı anlat\n");
}

void loop() {
    // Ana döngü boş - testler setup'ta yapıldı
    
    // Her 10 saniyede bir küçük bir mesaj göster
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 10000) {
        lastMsg = millis();
        Serial.println("Sistem çalışıyor... (Test tamamlandı)");
    }
    
    delay(100);
}
