# Vakitmatik P4 LED Panel - Görev Listesi

**Proje:** ESP32-S3 P4 LED Panel Kütüphanesi  
**Müşteri:** Umutcan Yılmaz  
**Geliştirici:** Veysel Karani Kılıçerkan  
**Başlangıç:** 25 Şubat 2026

---

## 📋 Görev Kategorileri

- 🔴 **Kritik Öncelik** - Proje için zorunlu
- 🟡 **Yüksek Öncelik** - MVP için gerekli
- 🟢 **Orta Öncelik** - Phase 1
- ⚪ **Düşük Öncelik** - Phase 2+

---

## Phase 0: Proje Altyapısı ✅

### TASK-001: Proje Yapısı Oluşturma 🔴
**Durum:** ✅ Tamamlandı  
**Süre:** 1 saat  
**Bağımlılık:** Yok

**Alt Görevler:**
- [x] GitHub repository oluştur
- [x] README.md hazırla
- [x] .gitignore yapılandır
- [x] PRD (Product Requirements Document) oluştur
- [x] Task listesi oluştur
- [ ] PlatformIO proje yapısı hazırla
- [ ] Klasör yapısını oluştur

**Not:** Repository: https://github.com/veyselka/Vakitmatik-P4-LED

---

### TASK-002: PlatformIO Kurulumu ve Konfigürasyon 🔴
**Durum:** 🔄 Bekliyor  
**Süre:** 2 saat  
**Bağımlılık:** TASK-001

**Alt Görevler:**
- [ ] `platformio.ini` dosyası oluştur
- [ ] ESP32-S3 board konfigürasyonu
- [ ] PSRAM build flag'lerini ekle
- [ ] HUB75 DMA kütüphanesini lib_deps'e ekle
- [ ] Pin tanımlamalarını yap
- [ ] İlk derleme testini yap

**platformio.ini İçeriği:**
```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

build_flags = 
    -DBOARD_HAS_PSRAM
    -DCONFIG_SPIRAM_CACHE_WORKAROUND
    
lib_deps = 
    mrfaptastic/ESP32 HUB75 LED MATRIX PANEL DMA Display

monitor_speed = 115200
upload_speed = 921600
```

**Kabul Kriteri:**
- [x] Proje hatasız derleniyor
- [ ] Upload başarılı
- [ ] Serial monitor çalışıyor

---

## Phase 1: Temel Kütüphane İskeletı 🔴

### TASK-003: HUB75 DMA Başlatma 🔴
**Durum:** 🔄 Bekliyor  
**Süre:** 3 saat  
**Bağımlılık:** TASK-002

**Alt Görevler:**
- [ ] `ESP32_P4_Matrix.h` header dosyası oluştur
- [ ] `ESP32_P4_Matrix.cpp` implementation oluştur
- [ ] HUB75 pin mapping tanımla
- [ ] MatrixPanel_I2S_DMA nesnesi başlat
- [ ] 18 panel zincirleme konfigürasyonu
- [ ] PSRAM frame buffer tahsisi

**Örnek Kod:**
```cpp
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#define PANEL_WIDTH 80
#define PANEL_HEIGHT 40
#define PANELS_X 3
#define PANELS_Y 6
#define TOTAL_WIDTH (PANEL_WIDTH * PANELS_X)   // 240
#define TOTAL_HEIGHT (PANEL_HEIGHT * PANELS_Y) // 240

MatrixPanel_I2S_DMA *dma_display = nullptr;

void setupMatrix() {
    HUB75_I2S_CFG::i2s_pins _pins = {
        R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
        A_PIN, B_PIN, C_PIN, D_PIN, E_PIN,
        LAT_PIN, OE_PIN, CLK_PIN
    };

    HUB75_I2S_CFG mxconfig(
        PANEL_WIDTH,
        PANEL_HEIGHT,
        PANELS_X * PANELS_Y,
        _pins
    );

    mxconfig.clkphase = false;
    mxconfig.driver = HUB75_I2S_CFG::ICN2038S; // veya ICN2037
    
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
}
```

**Kabul Kriteri:**
- [ ] Paneller ışığı yanıyor
- [ ] Flickering yok
- [ ] PSRAM kullanımı heap'de görünüyor

---

### TASK-004: 1/10 Scan Folded Matrix Custom Mapping 🔴
**Durum:** 🔄 Bekliyor  
**Süre:** 8 saat  
**Bağımlılık:** TASK-003

**Alt Görevler:**
- [ ] `CustomMapping.h` ve `.cpp` oluştur
- [ ] `VirtualMatrixPanel` sınıfını extend et
- [ ] 1/10 scan koordinat dönüşüm algoritması
- [ ] Panel 0-17 arası fiziksel mapping
- [ ] Test pattern'leri oluştur (grid, şerit)
- [ ] Koordinat doğrulama testleri

**Kritik Algoritma:**
```cpp
class CustomVirtualMatrixPanel : public VirtualMatrixPanel {
public:
    void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) override {
        // 240x240 -> 18 panel mapping
        int panel_x = x / 80;  // 0, 1, 2
        int panel_y = y / 40;  // 0-5
        int local_x = x % 80;
        int local_y = y % 40;
        
        // 1/10 Scan Folded Matrix mantığı
        int scan_row = local_y / 10;      // 0-3 (4 scan row)
        int fold_offset = local_y % 10;   // 0-9
        
        // ICN2037BP mapping (bu değerleri müşteri testinde ayarlayacağız)
        int physical_x = local_x;
        int physical_y = (scan_row * 10) + fold_offset;
        
        // Paneli bul (3x6 yerleşim)
        int panel_index = panel_y * 3 + panel_x;
        
        // Fiziksel koordinata çevir ve çiz
        matrix->drawPixelRGB888(
            panel_index * 80 + physical_x,
            physical_y,
            r, g, b
        );
    }
};
```

**Test Senaryoları:**
1. Tek pixel testi - (0,0), (239,239), (120,120)
2. Yatay çizgi - y=120
3. Dikey çizgi - x=120
4. Çapraz çizgi
5. Grid pattern (10px aralıklı)

**Kabul Kriteri:**
- [ ] (0,0) sol üst köşede yanıyor
- [ ] (239,239) sağ alt köşede yanıyor
- [ ] Yatay/dikey çizgiler düz
- [ ] Grid bozulma yok

---

### TASK-005: RGB Renk Kontrolü ve Kalibrasyonu 🔴
**Durum:** 🔄 Bekliyor  
**Süre:** 4 saat  
**Bağımlılık:** TASK-004

**Alt Görevler:**
- [ ] RGB pin sıralaması testleri (6 kombinasyon)
- [ ] Parlaklık ayarı (0-255)
- [ ] Renk paletleri tanımla
- [ ] Gamma correction uygula
- [ ] Renk test fonksiyonları

**Test Kodu:**
```cpp
void testColors() {
    // Kırmızı test
    dma_display->fillScreenRGB888(255, 0, 0);
    delay(1000);
    
    // Yeşil test
    dma_display->fillScreenRGB888(0, 255, 0);
    delay(1000);
    
    // Mavi test
    dma_display->fillScreenRGB888(0, 0, 255);
    delay(1000);
    
    // Beyaz test
    dma_display->fillScreenRGB888(255, 255, 255);
    delay(1000);
}
```

**Olası Sorun:**
Eğer renk kayması varsa `platformio.ini` veya kod içinde pin mapping'i değiştir:
```cpp
// Örnek: R ve G yer değiştirdiyse
_pins = {G1_PIN, R1_PIN, B1_PIN, G2_PIN, R2_PIN, B2_PIN, ...};
```

**Kabul Kriteri:**
- [ ] Kırmızı gerçekten kırmızı
- [ ] Yeşil doğru renk
- [ ] Mavi doğru renk
- [ ] Beyaz dengeli (sarımsı değil)

---

## Phase 2: Metin ve Font Desteği 🟡

### TASK-006: Türkçe Font Sistemi 🟡
**Durum:** 🔄 Bekliyor  
**Süre:** 6 saat  
**Bağımlılık:** TASK-005

**Alt Görevler:**
- [ ] `TurkishFont.h` dosyası oluştur
- [ ] UTF-8 encoding desteği
- [ ] Türkçe karakter bitmap'leri (ı, ğ, ü, ş, ö, ç, İ, Ğ, Ü, Ş, Ö, Ç)
- [ ] Font boyutları: 8px, 12px, 16px, 24px
- [ ] Font rendering engine
- [ ] Kerning (harf arası boşluk) ayarı

**Font Tanımlama:**
```cpp
// TurkishFont.h
const uint8_t font_8x8_turkish[][8] = {
    // 'İ' karakteri (0x130)
    {0x00, 0x00, 0x1C, 0x08, 0x08, 0x08, 0x1C, 0x00},
    // 'ı' karakteri (0x131)
    {0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x00},
    // ... diğer karakterler
};
```

**Test:**
```cpp
void testTurkishText() {
    dma_display->setTextColor(0xFFFF);
    dma_display->setCursor(10, 10);
    dma_display->print("İmsak");
    dma_display->setCursor(10, 30);
    dma_display->print("Güneş");
    dma_display->setCursor(10, 50);
    dma_display->print("Öğle");
}
```

**Kabul Kriteri:**
- [ ] Tüm Türkçe karakterler doğru görünüyor
- [ ] Harf arası boşluklar düzgün
- [ ] Farklı font boyutları çalışıyor

---

### TASK-007: Metin Gösterimi ve Hizalama 🟡
**Durum:** 🔄 Bekliyor  
**Süre:** 3 saat  
**Bağımlılık:** TASK-006

**Alt Görevler:**
- [ ] Sol hizalama
- [ ] Orta hizalama
- [ ] Sağ hizalama
- [ ] Scrolling text (opsiyonel)
- [ ] Metin sarma (word wrap)

**API:**
```cpp
class TextRenderer {
public:
    void drawText(const char* text, int x, int y, 
                  TextAlign align, int fontSize);
    void drawCentered(const char* text, int y, int fontSize);
    void scrollText(const char* text, int y, int delay_ms);
};
```

**Kabul Kriteri:**
- [ ] Metin merkezde hizalı
- [ ] Uzun metinler taşmıyor
- [ ] Scrolling çalışıyor (varsa)

---

## Phase 3: Vakıt Gösterimi 🟡

### TASK-008: Statik Vakıt Gösterimi 🟡
**Durum:** 🔄 Bekliyor  
**Süre:** 4 saat  
**Bağımlılık:** TASK-007

**Alt Görevler:**
- [ ] `VakitDisplay.h` ve `.cpp` oluştur
- [ ] Vakıt struct'ı tanımla
- [ ] Layout tasarımı (başlık, vakitler, saat)
- [ ] Renk şeması uygula
- [ ] Aktif vakit vurgulama
- [ ] Statik vakitlerle test

**Veri Yapısı:**
```cpp
struct PrayerTime {
    char name[10];     // "İmsak", "Güneş", vb.
    uint8_t hour;
    uint8_t minute;
    bool isPassed;
};

struct DailyPrayerTimes {
    PrayerTime imsak;
    PrayerTime gunes;
    PrayerTime ogle;
    PrayerTime ikindi;
    PrayerTime aksam;
    PrayerTime yatsi;
};
```

**Ekran Layout:**
```cpp
void drawPrayerTimes(DailyPrayerTimes times, int currentHour, int currentMin) {
    // Başlık
    drawCentered("NAMAZ VAKİTLERİ", 5, 12);
    drawCurrentTime(currentHour, currentMin);
    
    // Vakitler
    int y = 40;
    drawPrayerLine("İMSAK", times.imsak, y, isCurrentPrayer(...));
    y += 30;
    drawPrayerLine("GÜNEŞ", times.gunes, y, false);
    // ...
}
```

**Kabul Kriteri:**
- [ ] 6 vakit ekranda görünüyor
- [ ] Aktif vakit farklı renkle vurgulu
- [ ] Geçmiş vakitler soluk renkte
- [ ] Layout dengeli

---

### TASK-009: Saat Gösterimi 🟡
**Durum:** 🔄 Bekliyor  
**Süre:** 2 saat  
**Bağımlılık:** TASK-007

**Alt Görevler:**
- [ ] RTC (Real Time Clock) modülü entegrasyonu (varsa)
- [ ] millis() tabanlı saat sayacı
- [ ] Saat güncelleme fonksiyonu
- [ ] Tarih gösterimi (GG/AA/YYYY)
- [ ] 24 saat formatı

**Kod:**
```cpp
void displayClock() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 1000) {
        lastUpdate = millis();
        
        char timeStr[9];
        sprintf(timeStr, "%02d:%02d:%02d", hour, minute, second);
        
        drawCentered(timeStr, 10, 24);
        
        second++;
        if (second >= 60) {
            second = 0;
            minute++;
            if (minute >= 60) {
                minute = 0;
                hour++;
                if (hour >= 24) hour = 0;
            }
        }
    }
}
```

**Kabul Kriteri:**
- [ ] Saat her saniye güncelleniyor
- [ ] Zaman doğru ilerliyor (millis drift kontrolü)
- [ ] Format: HH:MM:SS

---

## Phase 4: Network ve API 🟢

### TASK-010: WiFi Bağlantısı 🟢
**Durum:** 🔄 Bekliyor  
**Süre:** 3 saat  
**Bağımlılık:** TASK-009

**Alt Görevler:**
- [ ] WiFi.h entegrasyonu
- [ ] SSID ve şifre konfigürasyonu
- [ ] Bağlantı durumu gösterimi (LED veya ekran)
- [ ] Otomatik yeniden bağlanma
- [ ] Timeout yönetimi

**Kod:**
```cpp
#include <WiFi.h>

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

void setupWiFi() {
    WiFi.begin(ssid, password);
    
    int timeout = 20; // 20 saniye
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        delay(1000);
        drawText("WiFi Bağlanıyor...", 10, 100);
        timeout--;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        drawText("WiFi Bağlı!", 10, 100);
        Serial.println(WiFi.localIP());
    }
}
```

**Kabul Kriteri:**
- [ ] WiFi başarıyla bağlanıyor
- [ ] IP adresi alınıyor
- [ ] Bağlantı kopunca yeniden deniyor

---

### TASK-011: NTP Zaman Senkronizasyonu 🟢
**Durum:** 🔄 Bekliyor  
**Süre:** 2 saat  
**Bağımlılık:** TASK-010

**Alt Görevler:**
- [ ] NTPClient kütüphanesi entegrasyonu
- [ ] NTP sunucusu ayarı (tr.pool.ntp.org)
- [ ] Timezone ayarı (UTC+3)
- [ ] Saatlik senkronizasyon
- [ ] Offline mod (NTP yoksa devam et)

**Kod:**
```cpp
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "tr.pool.ntp.org", 10800, 3600000); // UTC+3

void syncTime() {
    timeClient.update();
    
    int hours = timeClient.getHours();
    int minutes = timeClient.getMinutes();
    int seconds = timeClient.getSeconds();
    
    Serial.printf("NTP Saat: %02d:%02d:%02d\n", hours, minutes, seconds);
    
    // RTC güncelle veya global değişkenlere ata
}
```

**Kabul Kriteri:**
- [ ] İlk açılışta NTP senkronizasyonu yapılıyor
- [ ] Saat Türkiye saatiyle doğru
- [ ] Her saatte bir yenileme

---

### TASK-012: Namaz Vakitleri API Entegrasyonu 🟢
**Durum:** 🔄 Bekliyor  
**Süre:** 5 saat  
**Bağımlılık:** TASK-011

**Alt Görevler:**
- [ ] API seçimi (ezanvakti.herokuapp.com veya diyanet.gov.tr)
- [ ] HTTPClient ile API çağrısı
- [ ] JSON parsing (ArduinoJson)
- [ ] Şehir/ilçe konfigürasyonu
- [ ] Günlük vakitleri kaydet (SPIFFS/Preferences)
- [ ] Hata yönetimi (API down, timeout)

**API Örnek:**
```cpp
#include <HTTPClient.h>
#include <ArduinoJson.h>

String apiUrl = "https://ezanvakti.herokuapp.com/vakitler/9541"; // İstanbul

void fetchPrayerTimes() {
    HTTPClient http;
    http.begin(apiUrl);
    
    int httpCode = http.GET();
    if (httpCode == 200) {
        String payload = http.getString();
        
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, payload);
        
        const char* imsak = doc[0]["Imsak"];
        const char* gunes = doc[0]["Gunes"];
        // ... diğer vakitler
        
        // Vakitleri global struct'a kaydet
        savePrayerTimes(imsak, gunes, ...);
    }
    http.end();
}
```

**Kabul Kriteri:**
- [ ] Vakitler API'den başarıyla çekiliyor
- [ ] JSON doğru parse ediliyor
- [ ] Günlük otomatik güncelleme (00:00'da)
- [ ] Offline mod çalışıyor (önceki gün vakitleri)

---

## Phase 5: Optimizasyon ve İyileştirmeler ⚪

### TASK-013: DMA ve Flickering Optimizasyonu ⚪
**Durum:** 🔄 Bekliyor  
**Süre:** 4 saat  
**Bağımlılık:** TASK-005

**Alt Görevler:**
- [ ] clkphase ayarları dene
- [ ] latch_blanking değerlerini test et
- [ ] Driver IC seçimi (ICN2037BP vs ICN2038S)
- [ ] Refresh rate optimizasyonu
- [ ] Brightness PWM ince ayarı

**Test Konfigürasyonları:**
```cpp
// Config 1
mxconfig.clkphase = false;
mxconfig.latch_blanking = 4;
mxconfig.driver = HUB75_I2S_CFG::ICN2038S;

// Config 2
mxconfig.clkphase = true;
mxconfig.latch_blanking = 1;
mxconfig.driver = HUB75_I2S_CFG::ICN2037;

// Müşteri testinde en iyi kombinasyonu bul
```

**Kabul Kriteri:**
- [ ] Flickering tamamen yok
- [ ] Parlaklık dengeli
- [ ] Renk geçişleri düzgün

---

### TASK-014: Bellek Optimizasyonu ⚪
**Durum:** 🔄 Bekliyor  
**Süre:** 3 saat  
**Bağımlılık:** TASK-006

**Alt Görevler:**
- [ ] Font dosyalarını sıkıştır
- [ ] Kullanılmayan karakterleri çıkar
- [ ] PROGMEM kullan (Flash'a kaydet)
- [ ] Heap fragmentation kontrolü
- [ ] Stack overflow kontrolü

**Kod:**
```cpp
// Font'u Flash'a al
const uint8_t PROGMEM turkishFont[][8] = { ... };

// Kullanırken
uint8_t char_data[8];
memcpy_P(char_data, &turkishFont[charIndex], 8);
```

**Kabul Kriteri:**
- [ ] Heap kullanımı <50KB
- [ ] PSRAM kullanımı <1MB
- [ ] Uzun süreli çalışmada bellek sızıntısı yok

---

### TASK-015: Animasyonlar ve Geçişler ⚪
**Durum:** 🔄 Bekliyor  
**Süre:** 6 saat  
**Bağımlılık:** TASK-008

**Alt Görevler:**
- [ ] Fade in/out efektleri
- [ ] Vakit geçiş animasyonu
- [ ] Scrolling text (kayan yazı)
- [ ] Countdown (vakit geri sayımı)
- [ ] Renk geçişleri (gradient)

**Efekt Örnekleri:**
```cpp
void fadeIn(int duration_ms) {
    for (int brightness = 0; brightness <= 255; brightness += 5) {
        dma_display->setBrightness8(brightness);
        delay(duration_ms / 51);
    }
}

void countdownToNextPrayer() {
    int secondsLeft = calculateSecondsUntilNext();
    
    int hours = secondsLeft / 3600;
    int minutes = (secondsLeft % 3600) / 60;
    int secs = secondsLeft % 60;
    
    char countdown[12];
    sprintf(countdown, "%02d:%02d:%02d", hours, minutes, secs);
    drawText(countdown, 10, 200);
}
```

**Kabul Kriteri:**
- [ ] Animasyonlar pürüzsüz
- [ ] FPS istikrarlı
- [ ] CPU kullanımı <%50

---

## Phase 6: Test ve Deployment 🔴

### TASK-016: Uzaktan Donanım Testi 🔴
**Durum:** 🔄 Bekliyor  
**Süre:** 8 saat (iteratif)  
**Bağımlılık:** TASK-005

**Test Prosedürü:**
1. **Geliştirici:**
   - VS Code'da kod yaz
   - .bin dosyası derle
   - AnyDesk/TeamViewer ile müşteriye bağlan

2. **Müşteri:**
   - ESP32-S3 kartını USB'ye tak
   - Geliştirici'nin yüklediği kodu çalıştır
   - Fotoğraf/video çek
   - Gözlemleri bildir

3. **İterasyon:**
   - Renk kayması var mı? → Pin mapping değiştir
   - Görüntü bölünüyor mu? → Folded matrix ayarla
   - Titriyor mu? → DMA ayarları değiştir

**Test Senaryoları:**
- [ ] RGB renk testi
- [ ] Pixel mapping testi
- [ ] Metin gösterimi
- [ ] Vakıt ekranı
- [ ] 24 saat stress test

**Kabul Kriteri:**
- [ ] Tüm paneller doğru çalışıyor
- [ ] Renk düzgün
- [ ] Koordinatlar doğru
- [ ] Titreşim yok
- [ ] 24 saat sorunsuz çalışma

---

### TASK-017: Dokümantasyon 🟡
**Durum:** 🔄 Bekliyor  
**Süre:** 4 saat  
**Bağımlılık:** TASK-016

**Alt Görevler:**
- [ ] API Referans Kılavuzu
- [ ] Kurulum Talimatları
- [ ] Pin Bağlantı Şeması (Fritzing diyagramı)
- [ ] Sorun Giderme Kılavuzu
- [ ] Örnek Kodlar
- [ ] Video Tutoryal (opsiyonel)

**Kılavuz İçeriği:**
```markdown
# Kurulum Kılavuzu

## 1. Gerekli Malzemeler
- ESP32-S3 N16R8
- 18 adet P4 LED Panel (80x40, 1/10 scan)
- 5V 60A Güç Kaynağı
- HUB75 Kablolar

## 2. Donanım Bağlantısı
[Fritzing diyagramı]

## 3. Yazılım Kurulumu
```bash
# PlatformIO CLI
pio run -t upload
```

## 4. İlk Çalıştırma
...
```

**Kabul Kriteri:**
- [ ] Tüm dokümantasyon tamamlandı
- [ ] Örnekler test edildi
- [ ] README.md güncel

---

### TASK-018: Örnek Projeler 🟡
**Durum:** 🔄 Bekliyor  
**Süre:** 3 saat  
**Bağımlılık:** TASK-016

**Examples:**
```
examples/
├── 01_BasicTest/          # Temel panel testi
├── 02_ColorTest/          # RGB renk testi
├── 03_TextTest/           # Türkçe metin testi
├── 04_ClockDisplay/       # Saat gösterimi
├── 05_Vakitmatik/         # Komple vakıt projesi
└── 06_AnimationDemo/      # Animasyon örnekleri
```

**Kabul Kriteri:**
- [ ] Her örnek derleniyor
- [ ] README.md her örnekte mevcut
- [ ] Kod açıklamalı

---

## Phase 7: İleri Özellikler (Opsiyonel) ⚪

### TASK-019: Web Interface ⚪
**Durum:** 🔄 Bekliyor  
**Süre:** 8 saat  
**Bağımlılık:** TASK-012

**Özellikler:**
- [ ] ESP32 Web Server
- [ ] WiFi ayarları (SSID, Password)
- [ ] Şehir/ilçe seçimi
- [ ] Manuel vakıt girişi
- [ ] Parlaklık ayarı
- [ ] Tema seçimi
- [ ] OTA (Over-The-Air) güncelleme

---

### TASK-020: Ses ve Ezan ⚪
**Durum:** 🔄 Bekliyor  
**Süre:** 6 saat  
**Bağımlılık:** TASK-008

**Özellikler:**
- [ ] Buzzer/Speaker entegrasyonu
- [ ] MP3 modülü (DFPlayer Mini)
- [ ] Ezan sesi çalma
- [ ] Ses seviyesi kontrolü
- [ ] Sessiz mod

---

## 📊 İlerleme Özeti

| Phase | Görev Sayısı | Tamamlanan | Devam Eden | Bekleyen | İlerleme |
|-------|--------------|------------|------------|----------|----------|
| 0 - Altyapı | 2 | 1 | 1 | 0 | 50% |
| 1 - Temel | 3 | 0 | 0 | 3 | 0% |
| 2 - Font | 2 | 0 | 0 | 2 | 0% |
| 3 - Vakıt | 2 | 0 | 0 | 2 | 0% |
| 4 - Network | 3 | 0 | 0 | 3 | 0% |
| 5 - Optimizasyon | 3 | 0 | 0 | 3 | 0% |
| 6 - Test | 3 | 0 | 0 | 3 | 0% |
| 7 - İleri | 2 | 0 | 0 | 2 | 0% |
| **TOPLAM** | **20** | **1** | **1** | **18** | **5%** |

---

## ⏱️ Tahm

ini Çalışma Süresi

| Phase | Minimum | Maksimum |
|-------|---------|----------|
| Phase 0-1 | 10 saat | 15 saat |
| Phase 2-3 | 15 saat | 20 saat |
| Phase 4 | 10 saat | 15 saat |
| Phase 5-6 | 15 saat | 25 saat |
| Phase 7 (Opsiyonel) | 14 saat | 20 saat |
| **Toplam (MVP)** | **50 saat** | **75 saat** |
| **Toplam (Tam)** | **64 saat** | **95 saat** |

---

## 🚨 Kritik Yollar (Critical Path)

```
TASK-001 → TASK-002 → TASK-003 → TASK-004 → TASK-005 → TASK-016
```

Bu görevler sırayla tamamlanmalı, paralel ilerleme yapılamaz.

---

## 📝 Notlar

- Her task tamamlandığında GitHub'a commit yapılacak
- Müşteri testleri sonrası feedback toplantısı yapılacak
- Haftalık ilerleme raporu hazırlanacak
- Sorunlar ISSUES.md dosyasına kaydedilecek

---

**Son Güncelleme:** 25 Şubat 2026  
**Hazırlayan:** Veysel Karani Kılıçerkan
