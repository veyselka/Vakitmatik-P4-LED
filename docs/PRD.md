# Product Requirements Document (PRD)
## Vakitmatik P4 LED Panel Kütüphanesi

**Proje Sahibi:** Umutcan Yılmaz  
**Geliştirici:** Veysel Karani Kılıçerkan  
**Versiyon:** 1.0  
**Tarih:** 25 Şubat 2026

---

## 1. Proje Genel Bakış

### 1.1 Amaç
ESP32-S3 mikrodenetleyici kullanılarak 18 adet P4 LED panel ile 240x240 piksel çözünürlükte namaz vakitleri gösteren bir kütüphane geliştirmek.

### 1.2 Kapsam
- ESP32-S3 N16R8 (16MB Flash, 8MB PSRAM) donanımı için optimize edilmiş kütüphane
- HUB75 DMA protokolü ile yüksek performanslı görüntü aktarımı
- 1/10 Scan oranına sahip P4 paneller için özel mapping algoritması
- Türkçe karakter desteği
- Namaz vakitleri, tarih ve saat gösterimi

### 1.3 Hedef Donanım
- **Mikrodenetleyici:** ESP32-S3 N16R8
- **Panel Sayısı:** 18 adet
- **Panel Tipi:** P4 (80x40 piksel/panel)
- **Toplam Çözünürlük:** 240x240 piksel
- **Yerleşim:** 3 sütun × 6 satır (zincirleme bağlantı)
- **Scan Oranı:** 1/10
- **Sürücü IC:** ICN2037BP
- **İletişim:** HUB75 DMA

---

## 2. Teknik Gereksinimler

### 2.1 Donanım Gereksinimleri

#### 2.1.1 ESP32-S3 Özellikleri
```
- Flash: 16MB
- PSRAM: 8MB (Kritik: Frame buffer için zorunlu)
- WiFi: Entegre (Vakitleri çekmek için)
- İşlemci: Dual-core Xtensa LX7
- Frekans: 240 MHz
```

#### 2.1.2 Panel Bağlantı Şeması
```
Panel Dizilimi (Tavandan Bakış):
┌─────────┬─────────┬─────────┐
│ Panel 1 │ Panel 2 │ Panel 3 │  Satır 1
├─────────┼─────────┼─────────┤
│ Panel 4 │ Panel 5 │ Panel 6 │  Satır 2
├─────────┼─────────┼─────────┤
│ Panel 7 │ Panel 8 │ Panel 9 │  Satır 3
├─────────┼─────────┼─────────┤
│Panel 10 │Panel 11 │Panel 12 │  Satır 4
├─────────┼─────────┼─────────┤
│Panel 13 │Panel 14 │Panel 15 │  Satır 5
├─────────┼─────────┼─────────┤
│Panel 16 │Panel 17 │Panel 18 │  Satır 6
└─────────┴─────────┴─────────┘
   80px      80px      80px
   
Total: 240 x 240 piksel
```

#### 2.1.3 HUB75 Pin Bağlantıları
```cpp
// ESP32-S3 Pin Mapping (platformio.ini)
#define R1_PIN  25
#define G1_PIN  26
#define B1_PIN  27
#define R2_PIN  14
#define G2_PIN  12
#define B2_PIN  13
#define A_PIN   23
#define B_PIN   22
#define C_PIN   5
#define D_PIN   17
#define E_PIN   -1  // 1/10 scan, E pini kullanılabilir
#define LAT_PIN 4
#define OE_PIN  15
#define CLK_PIN 16
```

### 2.2 Yazılım Gereksinimleri

#### 2.2.1 Geliştirme Ortamı
- **IDE:** Visual Studio Code 1.85+
- **Extension:** PlatformIO IDE 3.3+
- **C/C++:** Microsoft C/C++ Extension
- **AI Asistan:** GitHub Copilot

#### 2.2.2 PlatformIO Konfigürasyonu
```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

; PSRAM zorunlu - olmadan 240x240 frame buffer sığmaz
build_flags = 
    -DBOARD_HAS_PSRAM
    -DCONFIG_SPIRAM_CACHE_WORKAROUND
    
lib_deps = 
    mrfaptastic/ESP32 HUB75 LED MATRIX PANEL DMA Display

monitor_speed = 115200
upload_speed = 921600
```

#### 2.2.3 Bağımlılıklar
```
- ESP32 HUB75 LED MATRIX PANEL DMA Display (mrfaptastic)
- Arduino Framework
- WiFi Library (ESP32 entegre)
- NTPClient (Vakıt senkronizasyonu için)
```

---

## 3. Fonksiyonel Gereksinimler

### 3.1 Temel Özellikler

#### FR-001: Frame Buffer Yönetimi
**Öncelik:** Kritik  
**Açıklama:** 240x240 piksel RGB frame buffer'ı PSRAM'de tutulacak
```cpp
// Gerekli PSRAM Hesabı:
// 240 x 240 x 3 (RGB) = 172,800 bytes ≈ 169 KB
```

#### FR-002: 1/10 Scan Folded Matrix Mapping
**Öncelik:** Kritik  
**Açıklama:** Kullanıcının girdiği (X, Y) koordinatını, 1/10 scan katlanmış matris mantığına çevirecek özel algoritma
```cpp
// Pseudo-code:
CustomVirtualMapping(x, y):
    panel_x = x / 80
    panel_y = y / 40
    local_x = x % 80
    local_y = y % 40
    
    // 1/10 scan için Y koordinatını 10'a böl
    scan_row = local_y / 10
    folded_offset = local_y % 10
    
    // ICN2037BP sürücüsüne göre mapping
    return mapped_coordinates
```

#### FR-003: Zincirleme Panel Desteği
**Öncelik:** Yüksek  
**Açıklama:** 18 panel'i daisy-chain (zincirleme) bağlantı ile yönetme

#### FR-004: RGB Renk Kontrolü
**Öncelik:** Orta  
**Açıklama:**
- 24-bit RGB renk desteği
- Parlaklık ayarı (0-255)
- Renk paletleri (namaz vakitleri için)

#### FR-005: Türkçe Karakter Desteği
**Öncelik:** Yüksek  
**Açıklama:** 
- UTF-8 encoding
- Türkçe karakterler: ı, ğ, ü, ş, ö, ç, İ, Ğ, Ü, Ş, Ö, Ç
- Namaz isimleri: İmsak, Güneş, Öğle, İkindi, Akşam, Yatsı

#### FR-006: Metin Gösterimi
**Öncelik:** Yüksek  
**Açıklama:**
- Dinamik font boyutları
- Metin hizalama (sol, orta, sağ)
- Scrolling text desteği

### 3.2 Namaz Vakitleri Özellikleri

#### FR-007: Vakıt Gösterimi
**Öncelik:** Kritik  
**Açıklama:**
```
Ekran Yerleşimi:
┌──────────────────────────┐
│      00:00      │  Tarih │  (Üst: Saat)
├──────────────────────────┤
│   İMSAK    05:30         │
│   GÜNEŞ    07:15         │
│   ÖĞLE     12:45         │
│   İKİNDİ   15:30         │
│   AKŞAM    18:00         │
│   YATSI    19:45         │
└──────────────────────────┘
```

#### FR-008: WiFi ve NTP Senkronizasyonu
**Öncelik:** Yüksek  
**Açıklama:**
- WiFi bağlantısı  
- NTP ile zaman senkronizasyonu
- Namaz vakitleri API entegrasyonu

---

## 4. Teknik Kısıtlamalar ve Çözümler

### 4.1 Bilinen Sorunlar ve Çözümleri

#### Problem 1: Renk Kayması
**Belirti:** Kırmızı yerine yeşil yanıyor  
**Çözüm:** 
```cpp
// I2S pin mapping'i değiştir
// RGB sırasını test et: RGB -> RBG -> GRB -> GBR -> BRG -> BGR
```

#### Problem 2: Görüntü Bölünmesi / Kayması
**Belirti:** Yazılar ortadan ikiye bölünüyor  
**Çözüm:**
```cpp
// Folded Matrix mapping algoritmasını düzelt
// 1/10 scan için Y offset hesaplamasını kontrol et
```

#### Problem 3: Ekran Titremesi (Flickering)
**Belirti:** Sürekli titreşim  
**Çözüm:**
```cpp
// DMA refresh rate ayarla
mxconfig.clkphase = false;
mxconfig.latch_blanking = 4;
// ICN2037BP için clock hızını düşür
```

### 4.2 Uzaktan Test Stratejisi

**Gerekçe:** Elde donanım yok, Wokwi HUB75 DMA desteklemiyor

**Çözüm:**
1. Müşteri ESP32-S3 kartını teslim alacak
2. Geliştirici VS Code'da kod yazacak
3. .bin dosyası derlenecek
4. Uzak masaüstü (AnyDesk/TeamViewer) ile müşterinin PC'sine bağlanılacak
5. Kod yüklenecek ve fotoğraf/video ile sonuç analiz edilecek
6. İteratif düzeltmeler yapılacak

---

## 5. Performans Gereksinimleri

### 5.1 Hız
- **Refresh Rate:** Minimum 60Hz (10ms frame süresi)
- **DMA Transfer:** Kesintisiz, flickering yok
- **Boot Süresi:** Maksimum 3 saniye

### 5.2 Bellek
- **PSRAM Kullanımı:** <1MB (frame buffer + font cache)
- **Flash Kullanımı:** <2MB (kod + font dosyaları)
- **Heap Kullanımı:** <50KB

### 5.3 Güvenilirlik
- **WiFi Yeniden Bağlanma:** Otomatik
- **Saat Senkronizasyonu:** Saatte 1 kez NTP kontrolü
- **Watchdog:** 10 saniye timeout

---

## 6. Kullanıcı Arayüzü

### 6.1 Ekran Düzeni
```
┌─────────────────────────────────────────────┐
│           15:23          25 Şubat 2026      │ <- Başlık (40px)
├─────────────────────────────────────────────┤
│                                             │
│         İMSAK    ●  05:30                   │ <- 30px satır
│         GÜNEŞ       07:15                   │
│         ÖĞLE        12:45                   │
│         İKİNDİ      15:30                   │
│         AKŞAM       18:00                   │
│         YATSI       19:45                   │
│                                             │
│  ● = Mevcut Vakit İşareti                  │
└─────────────────────────────────────────────┘
```

### 6.2 Renk Şeması
```cpp
// Renk Tanımlamaları
#define COLOR_HEADER       0x001F  // Mavi
#define COLOR_CURRENT      0xF800  // Kırmızı (aktif vakit)
#define COLOR_NORMAL       0x07E0  // Yeşil (normal vakit)
#define COLOR_PASSED       0x7BEF  // Gri (geçmiş vakit)
#define COLOR_BACKGROUND   0x0000  // Siyah
```

---

## 7. Kabul Kriterleri

### 7.1 Minimum Viable Product (MVP)
- [ ] 18 panel doğru şekilde tanınıyor
- [ ] 240x240 frame buffer çalışıyor
- [ ] Temel metin gösterimi (İngilizce karakterler)
- [ ] RGB renk kontrolü çalışıyor
- [ ] Flickering yok

### 7.2 Phase 1 - Temel İşlevsellik
- [ ] Türkçe karakter desteği
- [ ] Namaz vakitleri statik gösteriliyor
- [ ] Saat gösterimi (manuel ayarla)

### 7.3 Phase 2 - Network Özellikleri
- [ ] WiFi bağlantısı
- [ ] NTP saat senkronizasyonu
- [ ] Namaz vakitleri API'den çekiliyor

### 7.4 Phase 3 - İleri Özellikler
- [ ] Animasyonlar (vakit geçişleri)
- [ ] Ezan ezani (buzzer/speaker ile)
- [ ] Web interface (ayarlar için)

---

## 8. Test Planı

### 8.1 Birim Testleri
```cpp
// Test Cases
TEST(FrameBuffer, PSRAMAllocation);
TEST(Mapping, FoldedMatrix_1_10_Scan);
TEST(Colors, RGB24bit);
TEST(Turkish, UTF8Encoding);
```

### 8.2 Entegrasyon Testleri
- HUB75 DMA ile iletişim
- Panel zincirleme bağlantı
- WiFi + NTP senkronizasyonu

### 8.3 Donanım Testleri (Müşteri Tarafında)
1. Renk testi (RGB paletleri)
2. Koordinat testi (pixel mapping)
3. Metin gösterimi
4. Uzun süreli çalışma (24 saat)

---

## 9. Riskler ve Azaltma Stratejileri

| Risk | Olasılık | Etki | Azaltma |
|------|----------|------|---------|
| 1/10 Scan mapping yanlış | Yüksek | Kritik | İteratif test, müşteri feedback |
| ICN2037BP uyumsuzluğu | Orta | Yüksek | Driver özelleştirme |
| PSRAM yetersizliği | Düşük | Kritik | N16R8 kart zorunlu |
| WiFi bağlantı problemi | Orta | Orta | Offline mod + retry logic |
| Font dosyası boyutu | Orta | Orta | Sadece gerekli karakterler |

---

## 10. Çıktılar

### 10.1 Teslim Edilecekler
1. **ESP32_P4_Matrix_Lib** Arduino kütüphanesi
2. **Vakitmatik** örnek proje (examples/ klasörü)
3. **Dokümantasyon:**
   - API referansı
   - Kurulum kılavuzu
   - Pin bağlantı şeması
   - Sorun giderme kılavuzu
4. **Test Raporları**

### 10.2 Kaynak Kod Yapısı
```
ESP32_P4_Matrix_Lib/
├── src/
│   ├── ESP32_P4_Matrix.h
│   ├── ESP32_P4_Matrix.cpp
│   ├── CustomMapping.h
│   ├── CustomMapping.cpp
│   ├── TurkishFont.h
│   └── VakitDisplay.cpp
├── examples/
│   ├── BasicTest/
│   ├── Vakitmatik/
│   └── ColorTest/
├── docs/
├── platformio.ini
└── README.md
```

---

## 11. Referanslar

- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [HUB75 Protocol Specification](https://www.sparkfun.com/datasheets/Components/LED/LED_Panel_-_HUB75.pdf)
- [ESP32 HUB75 LED MATRIX PANEL DMA Display Library](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA)
- [ICN2037BP Datasheet](http://www.suntechled.com/uploadfiles/ICN2037BP.pdf)

---

**Onay:** 
- [ ] Müşteri: Umutcan Yılmaz
- [ ] Geliştirici: Veysel Karani Kılıçerkan
- [ ] Tarih: _____________________
