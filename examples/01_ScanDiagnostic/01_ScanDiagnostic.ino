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
    // Deneme 1: SHIFTREG clk=true - en iyi sonucu veren konfigürasyon
    {true,  1, HUB75_I2S_CFG::HZ_10M, "SHIFTREG / clk=true  / latch=1", HUB75_I2S_CFG::SHIFTREG},
    // Deneme 2: SHIFTREG clk=false
    {false, 1, HUB75_I2S_CFG::HZ_10M, "SHIFTREG / clk=false / latch=1", HUB75_I2S_CFG::SHIFTREG},
    // Deneme 3: ICN2038S clk=false
    {false, 4, HUB75_I2S_CFG::HZ_10M, "ICN2038S / clk=false / latch=4", HUB75_I2S_CFG::ICN2038S},
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

// ====================================================================
// 1/10 SCAN FOLDED MATRIX KOORDİNAT DÖNÜŞÜMÜ
// P4 80x40 panel: logical y → physical y
// Her scan adresi 4 satırı aynı anda sürer (0,10,20,30 - 1,11,21,31 ...)
// Formül: physical_y = (logical_y % 4) * 10 + (logical_y / 4)
// ====================================================================
#define SCAN_GROUPS  4                           // 40 satir / 10 scan = 4 grup
#define SCAN_HEIGHT  (PANEL_HEIGHT / SCAN_GROUPS) // 10 satir per grup

inline int mapY(int y) {
    return (y % SCAN_GROUPS) * SCAN_HEIGHT + (y / SCAN_GROUPS);
}

void mappedPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= PANEL_WIDTH || y < 0 || y >= PANEL_HEIGHT) return;
    dma_display->drawPixelRGB888(x, mapY(y), r, g, b);
}

// ---- Yardımcı Fonksiyonlar ----

void fillRow(int y, uint8_t r, uint8_t g, uint8_t b) {
    for (int x = 0; x < PANEL_WIDTH; x++) {
        mappedPixel(x, y, r, g, b);
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
    Serial.println("  [FRAME] Cerceve ve kose pikselleri (MAPPED)");
    clearAll();

    for (int x = 0; x < PANEL_WIDTH; x++) {
        mappedPixel(x, 0, 255, 255, 255);
        mappedPixel(x, PANEL_HEIGHT - 1, 255, 255, 255);
    }
    for (int y = 0; y < PANEL_HEIGHT; y++) {
        mappedPixel(0, y, 255, 255, 255);
        mappedPixel(PANEL_WIDTH - 1, y, 255, 255, 255);
    }

    // Köşe pikselleri (renkli - tanımlama için)
    mappedPixel(0, 0,                              255, 0,   0);  // Sol üst: Kırmızı
    mappedPixel(PANEL_WIDTH - 1, 0,                  0, 255,  0);  // Sağ üst: Yeşil
    mappedPixel(0, PANEL_HEIGHT - 1,                 0,   0, 255);  // Sol alt: Mavi
    mappedPixel(PANEL_WIDTH - 1, PANEL_HEIGHT - 1, 255, 255,   0);  // Sağ alt: Sarı

    delay(5000);
    clearAll();
}

// ---- TEST: Dikey çizgiler ----
void testVerticalLines() {
    Serial.println("  [VLINES] Her 10 piksel dikey cizgi (MAPPED)");
    clearAll();
    for (int x = 0; x < PANEL_WIDTH; x += 10) {
        for (int y = 0; y < PANEL_HEIGHT; y++) {
            mappedPixel(x, y, 255, 128, 0);
        }
    }
    delay(4000);
    clearAll();
}

// ---- TEST: Yatay çizgiler (her 10 satırlık grup farklı renk) ----
void testHorizontalLines() {
    Serial.println("  [HLINES] Her 10 satirlik grup farkli renk (MAPPED)");
    clearAll();
    for (int y = 0; y < PANEL_HEIGHT; y++) {
        uint8_t r = (y < 10)              ? 255 : 0;
        uint8_t g = (y >= 10 && y < 20)   ? 255 : 0;
        uint8_t b = (y >= 20 && y < 30)   ? 255 : 0;
        uint8_t w = (y >= 30)              ? 200 : 0;
        for (int x = 0; x < PANEL_WIDTH; x++) {
            mappedPixel(x, y, r + w, g + w, b + w);
        }
    }
    // Beklenen: üst 1/4 kırmızı, 2/4 yeşil, 3/4 mavi, alt 1/4 beyazımsı
    delay(5000);
    clearAll();
}

// ---- TEST: Büyük tek "L" harfi (MAPPED) ----
void testBigL() {
    Serial.println("  [BIG-L] Buyuk L harfi - MAPPED koordinatlar");
    Serial.println("  Beklenen: Tek parcali duzgun L gorunmeli!");
    clearAll();

    // Dikey çizgi (sol): x=8..11, y=2..37
    for (int y = 2; y <= 37; y++) {
        for (int t = 0; t < 4; t++) {
            mappedPixel(8 + t, y, 255, 255, 255);
        }
    }

    // Yatay taban: x=8..55, y=34..37
    for (int x = 8; x <= 55; x++) {
        for (int t = 0; t < 4; t++) {
            mappedPixel(x, 34 + t, 255, 255, 255);
        }
    }

    Serial.println("  L cizildi. Fotograf cekin!");
    delay(10000); // 10 saniye bekle
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
    Serial.println("==============================================");
    Serial.flush();

    // PSRAM kontrolü - DMA buffer için zorunlu
    Serial.println("\nPSRAM kontrol ediliyor...");
    Serial.flush();
    if (psramFound()) {
        Serial.printf("  PSRAM bulundu: %d bytes serbest\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    } else {
        Serial.println("  PSRAM YOK - Arduino IDE'de PSRAM ayari gerekli!");
        Serial.println("  Arduino IDE: Tools -> PSRAM -> 'OPI PSRAM' sec");
        Serial.println("  Daha az bellek gerektiren modu deneniyor...");
    }
    Serial.flush();

    // Heap durumu
    Serial.printf("  Heap serbest: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    Serial.printf("  Toplam konfigurasyon: %d\n\n", CONFIG_COUNT);
    Serial.flush();
    
    delay(2000); // Kullanıcının Serial Monitor'u açması için bekle
    Serial.println("Basliyor...");
    Serial.flush();
}

void loop() {
    Serial.printf("\n----------------------------------------------\n");
    Serial.printf("KONFIG %d/%d: %s\n", configIndex + 1, CONFIG_COUNT, configs[configIndex].driverName);
    Serial.printf("Heap: %d bytes serbest\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    Serial.printf("----------------------------------------------\n");
    Serial.flush();
    delay(200); // flush'in gitmesi için

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

    // 2. Yatay gruplar (4 renk bandı - koordinat doğrulaması)
    testHorizontalLines();

    // 3. Çerçeve ve köşeler
    testFrame();

    // 4. Dikey çizgiler
    testVerticalLines();

    // 5. Row scan (satır sırası kontrolü)
    testRowScan();

    // 6. Büyük L harfi - ANA TEST
    testBigL();

    Serial.printf("\n✓ Konfig %d testi bitti.\n", configIndex + 1);
    Serial.println(">>> 10 saniye beklenecek, sonra sonraki konfig denenecek...\n");
    delay(10000);

    configIndex = (configIndex + 1) % CONFIG_COUNT;
}
