/**
 * 01_ScanDiagnostic - Tek Panel Scan TanÄ±lama
 * 
 * P4 80x40 1/10 Scan ICN2037BP
 * Panel baÅŸlatma sadece setup()'ta yapÄ±lÄ±r â€” DMA restart hatasÄ± Ã¶nlenir.
 * 
 * @author Veysel Karani KÄ±lÄ±Ã§erkan
 * @customer Umutcan YÄ±lmaz
 */

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// ====================================================================
// PANEL BOYUTLARI
// ====================================================================
#define PANEL_WIDTH  80
#define PANEL_HEIGHT 40
#define NUM_PANELS   1

// ====================================================================
// PIN TANIMLARI - Umutcan ESP32S3 Dev Module
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
#define E_PIN   -1
#define LAT_PIN  4
#define OE_PIN  15
#define CLK_PIN 16
// ====================================================================

MatrixPanel_I2S_DMA *dma_display = nullptr;

// ====================================================================
// 1/10 SCAN FOLDED MATRIX KOORDÄ°NAT DÃ–NÃœÅÃœMÃœ
// 40 satÄ±r / 10 scan = her scan adresi 4 satÄ±rÄ± sÃ¼rer
// Scan addr 0 â†’ fiziksel satÄ±r 0, 10, 20, 30
// Scan addr 1 â†’ fiziksel satÄ±r 1, 11, 21, 31  vb.
// DolayÄ±sÄ±yla buffer'da logical y=0 â†’ fiziksel 0, logical y=1 â†’ fiziksel 10
// FormÃ¼l: physical = (y / 10) + (y % 10) * 4
// ====================================================================
#define SCAN_LINES   10   // 1/10 scan
#define ROW_GROUPS   4    // 40 / 10 = 4 grup

inline int mapY(int y) {
    // logical y â†’ physical y
    // Ã¶rn: y=0  â†’ (0/10) + (0%10)*4  = 0 + 0  = 0
    // Ã¶rn: y=1  â†’ (1/10) + (1%10)*4  = 0 + 4  = 4
    // Ã¶rn: y=10 â†’ (10/10) + (10%10)*4 = 1 + 0  = 1
    return (y / SCAN_LINES) + (y % SCAN_LINES) * ROW_GROUPS;
}

void mp(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= PANEL_WIDTH || y < 0 || y >= PANEL_HEIGHT) return;
    dma_display->drawPixelRGB888(x, mapY(y), r, g, b);
}

void cls() { dma_display->clearScreen(); delay(20); }

// Ham drawPixelRGB888 - mapY OLMADAN direkt buffer satiri
void rawRow(int bufRow, uint8_t r, uint8_t g, uint8_t b) {
    for (int x = 0; x < PANEL_WIDTH; x++)
        dma_display->drawPixelRGB888(x, bufRow, r, g, b);
}

// ====================================================================
// TEST 0-A: 4 anahtar buffer satiri - YAVASH (2 saniye her biri)
// Buffer 0, 10, 20, 30 → hangi FIZIKSEL satirlarda cikiyor?
// Musteri: her satiri sayarak nerede oldugunu bildirmeli
// ====================================================================
void testKeyRows() {
    // Test edilecek buffer satirlari ve renkleri
    int testRows[]   = {0,  1,  2,  10, 11, 20, 30};
    uint8_t cols[7][3] = {
        {255,0,0},    // 0: Kirmizi
        {0,255,0},    // 1: Yesil
        {0,0,255},    // 2: Mavi
        {255,255,0},  // 10: Sari
        {255,0,255},  // 11: Mor
        {0,255,255},  // 20: Cyan
        {255,128,0},  // 30: Turuncu
    };
    const char* names[] = {"0=KIRMIZI","1=YESIL","2=MAVI","10=SARI","11=MOR","20=CYAN","30=TURUNCU"};

    Serial.println("[0-A] ANAHTAR SATIR TESTI (mapY YOK)");
    Serial.println("  Her buffer satiri 2 saniye yanacak - SAYARAK hangi fiziksel satirda?");
    Serial.println("  Panel USTUnden asagiya: 1.satir=0, son=39");

    for (int i = 0; i < 7; i++) {
        cls();
        rawRow(testRows[i], cols[i][0], cols[i][1], cols[i][2]);
        Serial.printf("  >>> Buffer satir %s - HANGI FIZIKSEL SATIRDA? (kac tane yaniyor?)\n", names[i]);
        delay(2500);
    }
    cls();
    Serial.println("  [0-A] BITTI");
}

// ====================================================================
// TEST 0-B: Satir ciftleri karsilastirma
// Buffer 0 vs 10 vs 20 vs 30 → ayni fiziksel yere mi gidiyor?
// ====================================================================
void testRowPairs() {
    Serial.println("[0-B] SATIR CIFT KARSILASTIRMA");

    // 0 + 10 ayni anda
    cls();
    rawRow(0,  255, 0, 0);   // Kirmizi
    rawRow(10, 0, 255, 0);   // Yesil
    Serial.println("  Buf 0=KIRMIZI + Buf 10=YESIL ayni anda - kac renkli band goruyorsunuz?");
    Serial.println("  2 ayri renk = ayri fiziksel satir | 1 renk = aynı fiziksel satirlar catisiyor");
    delay(4000);

    cls();
    rawRow(0,  255, 0, 0);   // Kirmizi
    rawRow(20, 0, 0, 255);   // Mavi
    Serial.println("  Buf 0=KIRMIZI + Buf 20=MAVI - kac band?");
    delay(4000);

    cls();
    rawRow(0,  255, 0,   0);  // Kirmizi
    rawRow(10, 0,   255, 0);  // Yesil
    rawRow(20, 0,   0, 255);  // Mavi
    rawRow(30, 255, 255, 0);  // Sari
    Serial.println("  Buf 0+10+20+30 ayni anda - kac renkli band gordunuz? (4 ise = 4 ayri yer)");
    delay(5000);
    cls();
}

// ====================================================================
// MEVCUT TESTLER (mapY ile)

void testSolidColors() {
    Serial.println("[1] SOLID RENKLER");
    dma_display->fillScreenRGB888(255, 0, 0);   delay(1500);
    dma_display->fillScreenRGB888(0, 255, 0);   delay(1500);
    dma_display->fillScreenRGB888(0, 0, 255);   delay(1500);
    dma_display->fillScreenRGB888(255,255,255);  delay(1500);
    cls();
    Serial.println("  -> Tam ekran R/G/B/W yanmali. OK mi?");
}

void testHBands() {
    Serial.println("[2] YATAY RENK BANTLARI (MAPPED)");
    Serial.println("  Beklenen: ust=kirmizi / 2.ceyrek=yesil / 3.ceyrek=mavi / alt=beyaz");
    cls();
    for (int y = 0; y < PANEL_HEIGHT; y++) {
        uint8_t r=0, g=0, b=0;
        if      (y <  10) r = 255;
        else if (y <  20) g = 255;
        else if (y <  30) b = 255;
        else              { r=200; g=200; b=200; }
        for (int x = 0; x < PANEL_WIDTH; x++) mp(x, y, r, g, b);
    }
    delay(5000);
    cls();
}

void testFrame() {
    Serial.println("[3] CERCEVE (MAPPED)");
    Serial.println("  Beklenen: 4 koseli tek dikdortgen cerceve");
    cls();
    for (int x = 0; x < PANEL_WIDTH;  x++) {
        mp(x, 0,              255,255,255);
        mp(x, PANEL_HEIGHT-1, 255,255,255);
    }
    for (int y = 0; y < PANEL_HEIGHT; y++) {
        mp(0,             y, 255,255,255);
        mp(PANEL_WIDTH-1, y, 255,255,255);
    }
    // KÃ¶ÅŸe renkleri
    mp(0,             0,              255,  0,  0);  // Sol Ã¼st: kÄ±rmÄ±zÄ±
    mp(PANEL_WIDTH-1, 0,                0,255,  0);  // SaÄŸ Ã¼st: yeÅŸil
    mp(0,             PANEL_HEIGHT-1,   0,  0,255);  // Sol alt: mavi
    mp(PANEL_WIDTH-1, PANEL_HEIGHT-1, 255,255,  0);  // SaÄŸ alt: sarÄ±
    delay(6000);
    cls();
}

void testRowScan() {
    Serial.println("[4] ROW SCAN (MAPPED)");
    Serial.println("  Beklenen: yesil cizgi yukaridan asagiya tek iner");
    cls();
    for (int row = 0; row < PANEL_HEIGHT; row++) {
        cls();
        for (int x = 0; x < PANEL_WIDTH; x++) mp(x, row, 0, 255, 0);
        Serial.printf("  satir %2d\n", row);
        delay(150);
    }
    cls();
}

void testBigL() {
    Serial.println("[5] BUYUK L HARFI (MAPPED)");
    Serial.println("  Beklenen: tek parcali duzgun L");
    cls();
    // Dikey: x=8..11, y=1..38
    for (int y = 1; y <= 38; y++)
        for (int t = 0; t < 4; t++)
            mp(8+t, y, 255, 255, 255);
    // Taban: x=8..60, y=35..38
    for (int x = 8; x <= 60; x++)
        for (int t = 0; t < 4; t++)
            mp(x, 35+t, 255, 255, 255);
    Serial.println("  L cizildi - FOTOGRAF CEKIN!");
    delay(12000);
    cls();
}

// ====================================================================

void setup() {
    Serial.begin(115200);
    unsigned long t = millis();
    while (!Serial && millis() - t < 3000) delay(10);
    delay(500);

    Serial.println("\n==============================================");
    Serial.println("  01_ScanDiagnostic  |  ESP32-S3 P4 80x40");
    Serial.println("==============================================");

    if (psramFound())
        Serial.printf("  PSRAM: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    else
        Serial.println("  UYARI: PSRAM bulunamadi! Tools->PSRAM->OPI PSRAM sec.");

    Serial.printf("  Heap: %d bytes\n\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    Serial.flush();

    // ---- Panel baÅŸlatma (TEK SEFERLÄ°K) ----
    HUB75_I2S_CFG::i2s_pins _pins = {
        R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
        A_PIN, B_PIN, C_PIN, D_PIN, E_PIN,
        LAT_PIN, OE_PIN, CLK_PIN
    };

    HUB75_I2S_CFG mxconfig(PANEL_WIDTH, PANEL_HEIGHT, NUM_PANELS, _pins);
    mxconfig.clkphase       = true;
    mxconfig.driver         = HUB75_I2S_CFG::SHIFTREG;
    mxconfig.latch_blanking = 1;
    mxconfig.i2sspeed       = HUB75_I2S_CFG::HZ_10M;
    mxconfig.double_buff    = false;

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    if (!dma_display || !dma_display->begin()) {
        Serial.println("HATA: Panel baslatÄ±lamadi!");
        while(1) delay(1000);
    }

    dma_display->setBrightness8(180);
    dma_display->clearScreen();
    Serial.println("  Panel OK - SHIFTREG / clk=true / latch=1");
    Serial.println("\nTestler basliyor... (her test arasinda panel yanmali)\n");
    delay(1000);
}

void loop() {
    Serial.println("============ YENI TUR ============");

    // --- TANIMLAMA TESTLERI ---
    testKeyRows();   // Buffer 0,1,2,10,11,20,30 → tek tek 2.5sn, renk ile
    delay(1000);

    testRowPairs();  // 0+10+20+30 ayni anda → kac band?
    delay(1000);

    Serial.println("--- Simdi solid + mapped testler: ---\n");

    testSolidColors();
    delay(500);

    testHBands();
    delay(500);

    testFrame();
    delay(500);

    testBigL();
    delay(500);

    Serial.println("\nTur bitti. 10 saniye sonra yeniden basliyor...\n");
    delay(10000);
}
