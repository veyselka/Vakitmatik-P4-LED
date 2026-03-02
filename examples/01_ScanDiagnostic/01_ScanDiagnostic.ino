/**
 * 01_ScanDiagnostic - Tek Panel Scan Tanılama
 * 
 * AMAÇ: Panelin satır sırasının nasıl olduğunu anlamak.
 * 
 * BU PROGRAM NE YAPAR?
 *   Satırları tek tek, sırayla yakar (Row 0 → Row 1 → ... → Row 39)
 *   Eğer bir satır yaktığında fiziksel olarak başka bir satır yanıyorsa
 *   → scan mapping sorunu var demektir.
 * 
 * BEKLENEN SONUÇ:
 *   ✅ DOĞRU: Işık yukarıdan aşağıya tek satır halinde iner
 *   ❌ YANLIŞ: Işık atlayarak veya birden fazla satır aynı anda yanıyor
 * 
 * ====================================================================
 * MÜŞTERİ: PINLERİ KENDİ ESP32-S3'ÜNÜZE GÖRE DEĞİŞTİRİN
 * ====================================================================
 * 
 * @author Veysel Karani Kılıçerkan
 * @customer Umutcan Yılmaz
 */

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// ====================================================================
// PANEL BOYUTLARI - Bu panele göre sabit
// ====================================================================
#define PANEL_WIDTH  80
#define PANEL_HEIGHT 40
#define NUM_PANELS   1    // TEK PANEL

// ====================================================================
// PIN TANIMLARI - Umutcan'ın ESP32S3 Dev Module boarduna göre
// ====================================================================
#define R1_PIN   8
#define G1_PIN   9
#define B1_PIN  10
#define R2_PIN  14
#define G2_PIN  12
#define B2_PIN  13
#define A_PIN   20
#define B_PIN   21
#define C_PIN    5
#define D_PIN   17
#define E_PIN   -1    // 1/10 scan, E kullanılmıyor
#define LAT_PIN  4
#define OE_PIN  15
#define CLK_PIN 16
// ====================================================================

MatrixPanel_I2S_DMA *dma_display = nullptr;

// Konfigürasyon indexi - hangi sürücü/ayar deneniyor?
int configIndex = 0;

// Her testte denenecek driver konfigürasyonları
struct TestConfig {
    bool clkphase;
    int latch_blanking;
    HUB75_I2S_CFG::clk_speed i2sspeed;
    const char* driverName;
    HUB75_I2S_CFG::shift_driver driver;
};

TestConfig configs[] = {
    // Deneme 1: ICN2038S, varsayılan ayarlar
    {false, 4, HUB75_I2S_CFG::HZ_10M, "ICN2038S / clk=false / latch=4", HUB75_I2S_CFG::ICN2038S},
    // Deneme 2: ICN2038S, clkphase true
    {true,  4, HUB75_I2S_CFG::HZ_10M, "ICN2038S / clk=true  / latch=4", HUB75_I2S_CFG::ICN2038S},
    // Deneme 3: SHIFTREG (en temel sürücü), latch=1
    {false, 1, HUB75_I2S_CFG::HZ_10M, "SHIFTREG / clk=false / latch=1", HUB75_I2S_CFG::SHIFTREG},
    // Deneme 4: SHIFTREG, clkphase true
    {true,  1, HUB75_I2S_CFG::HZ_10M, "SHIFTREG / clk=true  / latch=1", HUB75_I2S_CFG::SHIFTREG},
    // Deneme 5: FM6126A (bazı P4 panellerde kullanılıyor)
    {false, 4, HUB75_I2S_CFG::HZ_10M, "FM6126A  / clk=false / latch=4", HUB75_I2S_CFG::FM6126A},
};
const int CONFIG_COUNT = sizeof(configs) / sizeof(configs[0]);

bool initPanel(int cfgIdx) {
    // Önceki display'i temizle
    if (dma_display != nullptr) {
        dma_display->stopDMAoutput();
        delete dma_display;
        dma_display = nullptr;
        delay(100);
    }

    HUB75_I2S_CFG::i2s_pins _pins = {
        R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
        A_PIN, B_PIN, C_PIN, D_PIN, E_PIN,
        LAT_PIN, OE_PIN, CLK_PIN
    };

    HUB75_I2S_CFG mxconfig(PANEL_WIDTH, PANEL_HEIGHT, NUM_PANELS, _pins);

    mxconfig.clkphase      = configs[cfgIdx].clkphase;
    mxconfig.driver        = configs[cfgIdx].driver;
    mxconfig.latch_blanking = configs[cfgIdx].latch_blanking;
    mxconfig.i2sspeed      = configs[cfgIdx].i2sspeed;
    mxconfig.double_buff   = false;

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    if (dma_display == nullptr) return false;
    if (!dma_display->begin()) {
        delete dma_display;
        dma_display = nullptr;
        return false;
    }

    dma_display->setBrightness8(200);
    dma_display->clearScreen();
    return true;
}

// ---- Yardımcı Fonksiyonlar ----

void fillRow(int y, uint8_t r, uint8_t g, uint8_t b) {
    for (int x = 0; x < PANEL_WIDTH; x++) {
        dma_display->drawPixelRGB888(x, y, r, g, b);
    }
}

void clearAll() {
    dma_display->clearScreen();
    delay(10);
}

// ---- TEST: Her satırı sırayla yak ----
void testRowScan() {
    Serial.println("  [ROW SCAN] Her satır sırayla yakılıyor...");
    Serial.println("  Beklenen: Işık yukarıdan aşağıya tek çizgi halinde inmeli");

    clearAll();
    for (int row = 0; row < PANEL_HEIGHT; row++) {
        clearAll();
        fillRow(row, 0, 255, 0);  // Yeşil
        Serial.printf("    Satır %2d yakıldı\n", row);
        delay(120);
    }
    clearAll();
}

// ---- TEST: Tüm ekran renk doldur ----
void testSolidColors() {
    Serial.println("  [SOLID] Kırmızı → Yeşil → Mavi → Beyaz");

    dma_display->fillScreenRGB888(255, 0, 0);
    delay(1500);
    dma_display->fillScreenRGB888(0, 255, 0);
    delay(1500);
    dma_display->fillScreenRGB888(0, 0, 255);
    delay(1500);
    dma_display->fillScreenRGB888(255, 255, 255);
    delay(1500);
    clearAll();
}

// ---- TEST: Çerçeve + köşe pikselleri ----
void testFrame() {
    Serial.println("  [FRAME] Çerçeve ve köşe pikselleri");
    clearAll();

    // Çerçeve
    for (int x = 0; x < PANEL_WIDTH; x++) {
        dma_display->drawPixelRGB888(x, 0, 255, 255, 255);
        dma_display->drawPixelRGB888(x, PANEL_HEIGHT - 1, 255, 255, 255);
    }
    for (int y = 0; y < PANEL_HEIGHT; y++) {
        dma_display->drawPixelRGB888(0, y, 255, 255, 255);
        dma_display->drawPixelRGB888(PANEL_WIDTH - 1, y, 255, 255, 255);
    }

    // Köşe pikselleri
    dma_display->drawPixelRGB888(0, 0,                     255, 0,   0);   // Sol üst: Kırmızı
    dma_display->drawPixelRGB888(PANEL_WIDTH - 1, 0,        0, 255,  0);   // Sağ üst: Yeşil
    dma_display->drawPixelRGB888(0, PANEL_HEIGHT - 1,       0,  0, 255);   // Sol alt: Mavi
    dma_display->drawPixelRGB888(PANEL_WIDTH - 1, PANEL_HEIGHT - 1, 255, 255, 0); // Sağ alt: Sarı

    delay(4000);
    clearAll();
}

// ---- TEST: Dikey çizgiler ----
void testVerticalLines() {
    Serial.println("  [VLINES] Her 10 piksel dikey çizgi");
    clearAll();
    for (int x = 0; x < PANEL_WIDTH; x += 10) {
        for (int y = 0; y < PANEL_HEIGHT; y++) {
            dma_display->drawPixelRGB888(x, y, 255, 128, 0);
        }
    }
    delay(4000);
    clearAll();
}

// ---- TEST: Yatay çizgiler ----
void testHorizontalLines() {
    Serial.println("  [HLINES] Her 4 piksel yatay çizgi");
    clearAll();
    for (int y = 0; y < PANEL_HEIGHT; y += 4) {
        for (int x = 0; x < PANEL_WIDTH; x++) {
            // Her grup farklı renk (scan gruplarını görselleştirmek için)
            int group = (y / 4) % 5;
            uint8_t r = (group == 0) ? 255 : 0;
            uint8_t g = (group == 1) ? 255 : 0;
            uint8_t b = (group == 2) ? 255 : 0;
            uint8_t w = (group == 3 || group == 4) ? 128 : 0;
            dma_display->drawPixelRGB888(x, y, r + w, g + w, b + w);
        }
    }
    delay(4000);
    clearAll();
}

// ---- TEST: Büyük tek "L" harfi (elle piksel) ----
void testBigL() {
    Serial.println("  [BIG-L] Büyük L harfi (10px geniş, 30px uzun)");
    clearAll();

    // Dikey çizgi: (10, 5) → (10, 35)
    for (int y = 5; y <= 35; y++) {
        for (int t = 0; t < 4; t++) {  // 4px kalınlık
            dma_display->drawPixelRGB888(10 + t, y, 255, 255, 255);
        }
    }

    // Yatay taban: (10, 32) → (40, 35)
    for (int x = 10; x <= 45; x++) {
        for (int t = 0; t < 4; t++) {  // 4px kalınlık
            dma_display->drawPixelRGB888(x, 32 + t, 255, 255, 255);
        }
    }

    delay(5000);
    clearAll();
}

// ====================================================================

void setup() {
    Serial.begin(115200);
    
    // ESP32-S3 USB CDC hazır olana kadar bekle (max 3 saniye)
    unsigned long t = millis();
    while (!Serial && millis() - t < 3000) { delay(10); }
    delay(500);

    Serial.println("\n\n==============================================");
    Serial.println("  01_ScanDiagnostic - Tek Panel Tanılama");
    Serial.println("  Panel: P4 80x40 1/10 Scan ICN2037BP");
    Serial.println("==============================================\n");

    Serial.printf("Toplam konfigürasyon denemesi: %d\n\n", CONFIG_COUNT);
}

void loop() {
    Serial.printf("\n----------------------------------------------\n");
    Serial.printf("KONFIG %d/%d: %s\n", configIndex + 1, CONFIG_COUNT, configs[configIndex].driverName);
    Serial.printf("----------------------------------------------\n");

    if (!initPanel(configIndex)) {
        Serial.println("HATA: Panel başlatılamadı! Bir sonraki konfig deneniyor...");
        configIndex = (configIndex + 1) % CONFIG_COUNT;
        delay(2000);
        return;
    }

    Serial.println("✓ Panel başatıldı\n");
    Serial.println("TESTLER:");

    // 1. Düz renk testi
    testSolidColors();

    // 2. Çerçeve ve köşeler
    testFrame();

    // 3. Yatay çizgiler (scan gruplarını görselleştirir)
    testHorizontalLines();

    // 4. Row scan (satır sırasını anlamak için)
    testRowScan();

    // 5. Büyük L harfi
    testBigL();

    Serial.printf("\n✓ Konfig %d testi bitti.\n", configIndex + 1);
    Serial.println(">>> 10 saniye beklenecek, sonra sonraki konfig denenecek...\n");
    delay(10000);

    configIndex = (configIndex + 1) % CONFIG_COUNT;
}
