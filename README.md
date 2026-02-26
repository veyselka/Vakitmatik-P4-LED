# Vakitmatik P4 LED Panel Kütüphanesi

ESP32-S3 tabanlı 18 panel (3x6) yerleşimindeki P4 LED matrisi için özel kütüphane.

## 📋 Proje Özeti

**Müşteri:** Umutcan Yılmaz  
**Donanım:** ESP32-S3 N16R8 (16MB Flash, 8MB PSRAM)  
**Panel Konfigürasyonu:** 18 adet 80x40 P4 panel (1/10 scan)  
**Toplam Çözünürlük:** 240x240 piksel  
**Sürücü IC:** ICN2037BP  
**Versiyon:** 1.0 (Tamamlandı)

## 🎯 Özellikler

- ✅ ESP32-S3 PSRAM desteği ile 240x240 frame buffer
- ✅ HUB75 DMA protokolü ile titremesiz görüntü
- ✅ 1/10 Scan Folded Matrix custom mapping desteği
- ✅ 18 panel zincirleme bağlantı desteği
- ✅ Türkçe karakter desteği ile namaz vakti gösterimi
- ✅ RGB kontrol ve renk ayarları
- ✅ WiFi bağlantısı ve NTP saat senkronizasyonu
- ✅ Otomatik namaz vakti gösterimi
- ✅ Büyük dijital saat ekranı
- ✅ Test pattern fonksiyonları

## 🛠️ Geliştirme Ortamı

- **IDE:** Visual Studio Code
- **Framework:** PlatformIO
- **Hedef Platform:** ESP32-S3

## 📦 Bağımlılıklar

```ini
lib_deps = 
    mrfaptastic/ESP32 HUB75 LED MATRIX PANEL DMA Display
    adafruit/Adafruit GFX Library @ ^1.11.9
    bblanchon/ArduinoJson @ ^7.0.0
    arduino-libraries/NTPClient @ ^3.2.1
```

## 🚀 Kurulum

### 1. PlatformIO Yükleme
```bash
# VS Code'a PlatformIO extension'ı yükle
# https://platformio.org/install/ide?install=vscode
```

### 2. Projeyi Aç
```bash
# Terminal'de proje klasörüne git
cd ESP32_P4_Matrix_Lib

# PlatformIO projesini başlat
pio init
```

### 3. Kütüphaneleri İndir
```bash
pio lib install
```

### 4. Derleme ve Yükleme
```bash
# Projeyi derle
pio run

# ESP32'ye yükle (COM port'unu değiştir)
pio run --target upload
```

## 📖 Kullanım

### Basit Örnek

```cpp
#include "ESP32_P4_Matrix.h"

ESP32_P4_Matrix matrix;

void setup() {
    // Matrix başlat
    matrix.begin();
    
    // WiFi bağlan
    matrix.connectWiFi("WiFi_Adi", "WiFi_Sifresi");
    
    // NTP saat senkronizasyonu
    matrix.syncTime();
    
    // Namaz vakitlerini ayarla
    PrayerTimes times;
    times.fajr = "05:45";
    times.dhuhr = "12:35";
    // ... diğer vakitler
    matrix.setPrayerTimes(times);
}

void loop() {
    // Dijital saat göster
    matrix.displayClock(20, 80, COLOR_GREEN);
    delay(1000);
}
```

### Namaz Vakitlerini Gösterme

```cpp
// Vakitleri göster
matrix.displayPrayerTimes(10, 10);
```

### Türkçe Metin Yazma

```cpp
// Türkçe karakterler otomatik dönüştürülür
matrix.drawTextTurkish("Namaz Vakti", 10, 10, COLOR_WHITE, 2);
```

## 🔧 Pin Bağlantıları

```cpp
// platformio.ini'de tanımlı
R1_PIN  = 25    G1_PIN = 26    B1_PIN = 27
R2_PIN  = 14    G2_PIN = 12    B2_PIN = 13
A_PIN   = 23    B_PIN  = 22    C_PIN  = 5
D_PIN   = 17    E_PIN  = -1
LAT_PIN = 4     OE_PIN = 15    CLK_PIN = 16
```

## 📁 Proje Yapısı

```
ESP32_P4_Matrix_Lib/
├── src/
│   ├── ESP32_P4_Matrix.h      # Kütüphane header
│   ├── ESP32_P4_Matrix.cpp    # Kütüphane implementasyon
│   └── main.cpp               # Ana program
├── examples/
│   └── 00_FirstTest/          # İlk test programı
│       └── 00_FirstTest.ino
├── docs/
│   ├── PRD.md                 # Proje gereksinimleri
│   ├── TASKS.md               # Görev listesi
│   └── rehberler/
│       ├── DERLEME_REHBERI.md
│       └── MUSTERI_HAZIRLIK_KILAVUZU.md
├── platformio.ini             # PlatformIO konfigürasyonu
└── README.md                  # Bu dosya
```

## 🧪 Test

İlk kurulumda `examples/00_FirstTest/00_FirstTest.ino` dosyasını yükleyin. Bu test:
- Tüm panellerin çalıştığını doğrular
- Renk kalibrasyonu yapar
- Pixel mapping'i test eder

## 📞 Destek

**Geliştirici:** Veysel Karani Kılıçerkan  
**E-posta:** veyselklcrn321@gmail.com  
**WhatsApp:** +90 507 840 93 23

## 📄 Lisans

Bu proje müşteri (Umutcan Yılmaz) için özel olarak geliştirilmiştir.

---

**Son Güncelleme:** 26 Şubat 2026  
**Durum:** ✅ Proje Tamamlandı
