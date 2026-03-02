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
// f10 = 1/10 scan → 10 scan adresi
// HEIGHT=40 → kutuphane 20 scan addr bekler, panel 10 tane var → YANLIS
// HEIGHT=20 → kutuphane 10 scan addr kullanir → DOGRU!
// Sonuc: fiziksel satir 0-19 gorulur, 20-39 onlarin aynasi olur (1/10 scan)
// WIDTH=40 + NUM=2 → toplam 80px, her half-panel 40px
// ====================================================================
#define PANEL_WIDTH  40   // Tek half-panel genisligi
#define PANEL_HEIGHT 20   // 20/2 = 10 scan line = f10 ile eslesir
#define NUM_PANELS   2    // Iki half-panel → toplam 80px genislik

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

// Toplam gorunen genislik
#define TOTAL_WIDTH  (PANEL_WIDTH * NUM_PANELS)  // 40*2 = 80
#define TOTAL_HEIGHT PANEL_HEIGHT               // 20
: kimlik - HEIGHT=20 ile scan artik dogru olmali
inline int mapY(int y) { return y; }

void mp(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= TOTAL_WIDTH || y < 0 || y >= TOTAL_HEIGHT) return;
    dma_display->drawPixelRGB888(x, mapY(y), r, g, b);
}

void cls() { dma_display->clearScreen(); delay(20); }

// Ham drawPixelRGB888 - TOTAL_WIDTH kadar
void rawRow(int bufRow, uint8_t r, uint8_t g, uint8_t b) {
    for (int x = 0; x < TOTAL_WIDTH; x++)
        dma_display->drawPixelRGB888(x, bufRow, r, g, b);
}

// ====================================================================
// TEST 0-A: 4 anahtar buffer satiri - YAVASH (2 saniye her biri)
// Buffer 0, 10, 20, 30 → hangi FIZIKSEL satirlarda cikiyor?
// Musteri: her satiri sayarak nerede oldugunu bildirmeli
// ====================================================================
void testKeyRows() {
    int testRows[]   = {0, 1, 2, 5, 9, 10, 15, 19};
    uint8_t cols[8][3] = {
        {255,0,0}, {0,255,0}, {0,0,255}, {255,255,0},
        {255,0,255}, {0,255,255}, {255,128,0}, {128,255,128}
    };
    const char* names[] = {"0","1","2","5","9","10","15","19"};

    Serial.println("[0-A] ANAHTAR SATIR (HEIGHT=20, mapY YOK)");
    Serial.println("  Her buffer satiri 2.5sn - kac fiziksel satir yaniyor?");
    for (int i = 0; i < 8; i++) {
        cls();
        rawRow(testRows[i], cols[i][0], cols[i][1], cols[i][2]);
        Serial.printf("  >>> Buf %s - kac satir yandi?\n", names[i]);
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
    Serial.println("[2] YATAY RENK BANTLARI (TOTAL 80x20)");
    Serial.println("  Beklenen: ust yarim=kirmizi / alt yarim=yesil");
    cls();
    for (int y = 0; y < TOTAL_HEIGHT; y++) {
        uint8_t r=0, g=0, b=0;
        if (y < TOTAL_HEIGHT/2) r = 255;
        else                    g = 255;
        for (int x = 0; x < TOTAL_WIDTH; x++) mp(x, y, r, g, b);
    }
    delay(5000);
    cls();
}

void testFrame() {
    Serial.println("[3] CERCEVE 80x20 (MAPPED)");
    Serial.println("  Beklenen: tek dikdortgen cerceve 80x20");
    cls();
    for (int x = 0; x < TOTAL_WIDTH;  x++) {
        mp(x, 0,               255,255,255);
        mp(x, TOTAL_HEIGHT-1,  255,255,255);
    }
    for (int y = 0; y < TOTAL_HEIGHT; y++) {
        mp(0,              y, 255,255,255);
        mp(TOTAL_WIDTH-1,  y, 255,255,255);
    }
    mp(0,             0,              255,  0,  0);  // Sol ust: kirmizi
    mp(TOTAL_WIDTH-1, 0,                0,255,  0);  // Sag ust: yesil
    mp(0,             TOTAL_HEIGHT-1,   0,  0,255);  // Sol alt: mavi
    mp(TOTAL_WIDTH-1, TOTAL_HEIGHT-1, 255,255,  0);  // Sag alt: sari
    delay(6000);
    cls();
}

void testRowScan() {
    Serial.println("[4] ROW SCAN 80x20 (MAPPED)");
    Serial.println("  Beklenen: yesil cizgi tek iner, 20 satir");
    cls();
    for (int row = 0; row < TOTAL_HEIGHT; row++) {
        cls();
        for (int x = 0; x < TOTAL_WIDTH; x++) mp(x, row, 0, 255, 0);
        Serial.printf("  satir %2d\n", row);
        delay(200);
    }
    cls();
}

void testBigL() {
    Serial.println("[5] BUYUK L HARFI 80x20 (MAPPED)");
    Serial.println("  Beklenen: tek parcali duzgun L, tum panel genisligi");
    cls();
    // Dikey: x=5..8, y=1..18  (3px kalin)
    for (int y = 1; y <= 18; y++)
        for (int t = 0; t < 3; t++)
            mp(5+t, y, 255, 255, 255);
    // Taban: x=5..74, y=16..18  (3px kalin)
    for (int x = 5; x <= 74; x++)
        for (int t = 0; t < 3; t++)
            mp(x, 16+t, 255, 255, 255);
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

    // NUM_PANELS=2, WIDTH=40 → toplam gorunen genislik = 40*2 = 80px
    HUB75_I2S_CFG mxconfig(PANEL_WIDTH, PANEL_HEIGHT, NUM_PANELS, _pins);
    mxconfig.clkphase       = true;
    mxconfig.driver         = HUB75_I2S_CFG::SHIFTREG;
    mxconfig.latch_blanking = 1;
    mxconfig.i2sspeed       = HUB75_I2S_CFG::HZ_10M;
    mxconfig.double_buff    = false;

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    if (!dma_display || !dma_display->begin()) {
        Serial.println("HATA: Panel baslatamadi!");
        while(1) delay(1000);
    }

    dma_display->setBrightness8(180);
    dma_display->clearScreen();
    Serial.printf("  Panel OK - %dx%d x%d panels\n", PANEL_WIDTH, PANEL_HEIGHT, NUM_PANELS);
    Serial.printf("  Gorunen toplam: %dx%d\n", PANEL_WIDTH * NUM_PANELS, PANEL_HEIGHT);
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
