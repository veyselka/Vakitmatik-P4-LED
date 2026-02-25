# Vakitmatik P4 LED Panel - Teslim Paketi

**Proje:** ESP32-S3 P4 LED Panel Vakıt Gösterimi  
**Geliştirici:** Veysel Karani Kılıçerkan  
**Müşteri:** Umutcan Yılmaz  
**Versiyon:** 1.0  
**Tarih:** 29 Şubat 2026

---

## 📦 İÇİNDEKİLER

Bu klasörde şunlar var:

```
delivery/
├── firmware/
│   ├── vakitmatik_v1.0.bin       ← ESP32'ye yüklenecek dosya
│   └── FLASH_NASIL.txt           ← Yükleme talimatları
│
├── source_code/
│   └── GitHub_Link.txt           ← Kaynak kod linki
│
├── docs/
│   ├── Kurulum_Kilavuzu.pdf      ← Donanım kurulumu
│   ├── Kullanim_Kilavuzu.pdf     ← Yazılım kullanımı
│   ├── Pin_Baglanti_Semasi.png   ← Kablolama şeması
│   └── Sorun_Giderme.pdf         ← Troubleshooting
│
├── video/
│   └── calisir_halde.mp4         ← Çalışır haldeki video kanıtı
│
└── README.txt                    ← BU DOSYA
```

---

## ⚡ HIZLI BAŞLANGIÇ

### 1. Donanımı Hazırla
- ESP32-S3 N16R8 kartı
- 18 adet P4 LED Panel (80x40, 1/10 scan)
- 5V 60A Güç kaynağı
- USB kablosu

### 2. Firmware Yükle
1. `firmware/` klasöründeki `vakitmatik_v1.0.bin` dosyasını bul
2. `FLASH_NASIL.txt` dosyasındaki adımları takip et
3. ESP32'ye yükle

### 3. İlk Çalıştırma
- Güç kaynağını aç
- ESP32'yi USB'ye tak
- Ekranda test pattern göreceksin (RGB renkler)
- 5 saniye sonra vakitler görünecek

---

## 📋 ÖZELLİKLER

✅ 240x240 piksel çözünürlük (18 panel)  
✅ HUB75 DMA ile titremesiz görüntü  
✅ Türkçe karakter desteği (İ, Ğ, Ü, Ş, Ö, Ç)  
✅ 6 vakit gösterimi (İmsak, Güneş, Öğle, İkindi, Akşam, Yatsı)  
✅ Saat ve tarih gösterimi  
✅ RGB renk kontrolü  
✅ Ayarlanabilir parlaklık  

⚪ Opsiyonel (ileride eklenebilir):  
- WiFi + NTP otomatik saat  
- Namaz vakitleri API entegrasyonu  
- Ezan sesi (buzzer/speaker ile)  
- Web arayüzü (ayarlar için)

---

## 🛠️ DESTEK

### Sorun mu var?

1. **İlk önce:** `docs/Sorun_Giderme.pdf` dosyasına bak
2. **Hala çözülmediyse:** Veysel Karani Kılıçerkan'a ulaş
   - Email: [email adresi]
   - WhatsApp: [telefon]
   - GitHub: https://github.com/veyselka/Vakitmatik-P4-LED

### Sık Karşılaşılan Sorunlar

❓ **Ekran ışık vermiyor**  
→ Güç kaynağı bağlı mı? USB bağlı mı?

❓ **Renkler yanlış (kırmızı yerine yeşil)**  
→ Pin mapping ayarı gerekebilir (bana ulaş)

❓ **Görüntü ortadan bölünmüş**  
→ 1/10 scan mapping ayarı (kod güncellemesi gerekebilir)

❓ **Ekran titriyor (flickering)**  
→ DMA ayarları optimize edilmeli (bana ulaş)

---

## 📄 LİSANS

Bu yazılım Umutcan Yılmaz için özel olarak geliştirilmiştir.  
Kaynak kodu açık (GitHub'da), ticari kullanım için izin gerekir.

---

## 🙏 TEŞEKKÜRLER

Vakitmatik P4 LED Panel projesini tercih ettiğiniz için teşekkürler!

**Geliştirici:**  
Veysel Karani Kılıçerkan  
GitHub: https://github.com/veyselka

**Müşteri:**  
Umutcan Yılmaz  

**Proje Deposu:**  
https://github.com/veyselka/Vakitmatik-P4-LED

---

**Son Güncelleme:** 29 Şubat 2026  
**Versiyon:** 1.0
