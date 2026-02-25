# Vakitmatik P4 LED Panel Kütüphanesi

ESP32-S3 tabanlı 18 panel (3x6) yerleşimindeki P4 LED matrisi için özel kütüphane.

## 📋 Proje Özeti

**Müşteri:** Umutcan Yılmaz  
**Donanım:** ESP32-S3 N16R8 (16MB Flash, 8MB PSRAM)  
**Panel Konfigürasyonu:** 18 adet 80x40 P4 panel (1/10 scan)  
**Toplam Çözünürlük:** 240x240 piksel  
**Sürücü IC:** ICN2037BP

## 🎯 Özellikler

- ✅ ESP32-S3 PSRAM desteği ile 240x240 frame buffer
- ✅ HUB75 DMA protokolü ile titremesiz görüntü
- ✅ 1/10 Scan Folded Matrix custom mapping
- ✅ 18 panel zincirleme bağlantı desteği
- ✅ Türkçe karakter desteği ile namaz vakti gösterimi
- ✅ RGB kontrol ve renk ayarları

## 🛠️ Geliştirme Ortamı

- **IDE:** Visual Studio Code
- **Framework:** PlatformIO
- **AI Asistan:** GitHub Copilot
- **Hedef Platform:** ESP32-S3

## 📦 Bağımlılıklar

```ini
lib_deps = 
    mrfaptastic/ESP32 HUB75 LED MATRIX PANEL DMA Display
```

## 🚀 Kurulum

Detaylı kurulum talimatları ve PlatformIO konfigürasyonu için [VS Code Geliştirme Gereksinimleri](VS%20Code%20Geliştirme%20Gereksinimleri.md) dosyasına bakınız.

## 📖 Dokümantasyon

- [Gereksinim Analizi Raporu](Gereksinim%20Analizi%20Raporu.pdf)
- [VS Code Geliştirme Gereksinimleri](VS%20Code%20Geliştirme%20Gereksinimleri.md)

## 👨‍💻 Geliştirici

**Veysel Karani Kılıçerkan**

## 📄 Lisans

Bu proje müşteri (Umutcan Yılmaz) için özel olarak geliştirilmiştir.
