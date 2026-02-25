# 🌐 UZAKTAN BAĞLANMA VE TEST REHBERİ

**Proje:** Vakitmatik P4 LED Panel  
**Geliştirici:** Veysel Karani Kılıçerkan  
**Müşteri:** Umutcan Yılmaz  
**Amaç:** RustDesk ile uzaktan bağlanıp ESP32'ye kod yükleme

---

## 📋 İÇİNDEKİLER

1. [Müşteri Hazırlık Süreci](#1-müşteri-hazirlik-süreci)
2. [Veysel'in Hazırlık Süreci](#2-veyselin-hazirlik-süreci)
3. [İlk Bağlantı Testi](#3-ilk-bağlanti-testi)
4. [Yazılım Kurulumları](#4-yazilim-kurulumlari)
5. [İlk Kod Yükleme](#5-ilk-kod-yükleme)
6. [Test Senaryoları](#6-test-senaryolari)
7. [Sorun Giderme](#7-sorun-giderme)

---

## 1️⃣ MÜŞTERİ HAZIRLIK SÜRECİ

### ADIM 1.1: Donanım Temin (MÜŞTERİ)

**Müşteri şunları satın alacak:**

✅ **ESP32-S3 N16R8 Geliştirme Kartı**
- Model: ESP32-S3 DevKit-C
- Özellikler: 16MB Flash, 8MB PSRAM
- USB-C kablosu (genellikle dahil)
- Nereden: Hepsiburada, AliExpress, Robotistan
- Fiyat: ~400-500 TL

✅ **18 adet P4 LED Panel**
- Boyut: 80×40 piksel
- Scan: 1/10 scan
- Sürücü: ICN2037BP veya ICN2038S
- Nereden: AliExpress (LED Matrix Indoor P4)

✅ **5V 60A Güç Kaynağı**
- Çıkış: 5V DC, 60A (300W)
- Konnektör: DC 5.5×2.5mm
- LED panel'ler için

✅ **Mini USB Kablo** (ESP32 için)

---

### ADIM 1.2: RustDesk Kurulumu (MÜŞTERİ)

#### 1.2.1 İndirme
1. Tarayıcıyı aç
2. Git: **https://rustdesk.com/**
3. "Download" butonuna tıkla
4. Windows için `.exe` dosyasını indir

#### 1.2.2 Kurulum
```
1. İndirilen rustdesk-1.x.x.exe dosyasına çift tıkla
2. "Run" / "Çalıştır" butonuna bas
3. Kurulum tipini seç: "Install for all users" (önerilen)
4. "Install" / "Kur" butonuna bas
5. Kurulum bitince otomatik açılacak
```

#### 1.2.3 İlk Ayarlar
```
RustDesk açıldığında şöyle görünecek:

┌────────────────────────────────┐
│  Your Desktop ID               │
│  ┌────────────────────┐        │
│  │  123 456 789       │ ◄──── BU NUMARAYI KOPYALA
│  └────────────────────┘        │
│                                │
│  Password                      │
│  ┌────────────────────┐        │
│  │  abcd1234          │ ◄──── BU ŞİFREYİ KOPYALA
│  └────────────────────┘        │
└────────────────────────────────┘
```

**Müşteri bu 2 bilgiyi sana gönderecek:**
- Desktop ID: `123 456 789`
- Password: `abcd1234`

#### 1.2.4 Şifre Değiştirme (Önerilen)
```
1. RustDesk penceresinde "Password" yanındaki ⚙️ ayar ikonuna tıkla
2. "Set permanent password" / "Kalıcı şifre belirle"
3. Kolay hatırlanır bir şifre gir (örn: vakitmatik2026)
4. "OK" / "Tamam" butonuna bas
5. YENİ ŞİFREYİ sana gönder
```

---

### ADIM 1.3: Python Kurulumu (MÜŞTERİ)

#### 1.3.1 Python İndirme
1. Git: **https://www.python.org/downloads/**
2. "Download Python 3.x.x" butonuna tıkla
3. İndirilen dosyayı çalıştır

#### 1.3.2 Python Kurulumu ⚠️ ÖNEMLİ
```
Kurulum ekranında:

[✓] Add Python to PATH  ◄──── BUNU MUTLAKA İŞARETLE!
[✓] Install for all users

"Install Now" butonuna bas

Kurulum bitince:
"Disable path length limit" ◄──── BUNA TIK AT
```

#### 1.3.3 Python Kontrol
Komut istemi aç (cmd veya PowerShell) ve yaz:
```bash
python --version
```

Çıktı:
```
Python 3.12.x
```

✅ Görünüyorsa tamam!  
❌ Görmüyorsa Python'u kaldır ve tekrar kur (PATH işaretle!)

---

### ADIM 1.4: PlatformIO Kurulumu (MÜŞTERİ)

#### 1.4.1 Komut İstemi Aç
```
1. Windows tuşuna bas
2. Yaz: "powershell" veya "cmd"
3. Enter'a bas
```

#### 1.4.2 PlatformIO Kur
Komuta yaz:
```bash
pip install platformio
```

Ekranda bir sürü yazı akacak (30-60 saniye):
```
Collecting platformio
  Downloading platformio-6.x.x.whl
Installing collected packages: ...
Successfully installed platformio-6.x.x
```

#### 1.4.3 Kontrol
```bash
pio --version
```

Çıktı:
```
PlatformIO Core, version 6.x.x
```

✅ Görünüyorsa TAMAM!

---

### ADIM 1.5: USB Sürücü Kurulumu (MÜŞTERİ)

#### 1.5.1 ESP32'yi Tak
```
1. ESP32-S3 kartını USB kablosu ile bilgisayara bağla
2. Windows "ding" sesi duyacaksın (cihaz tanındı)
```

#### 1.5.2 COM Port Kontrol
Komut isteminde:
```bash
pio device list
```

Çıktı:
```
COM3
----
Hardware ID: USB VID:PID=10C4:EA60
Description: Silicon Labs CP210x USB to UART Bridge (COM3)
```

✅ COM portu görünüyorsa sürücü kurulu!

#### 1.5.3 Sürücü Kurulmamışsa
Eğer "USB Cihazı tanınmadı" hatası varsa:

**CP210x Sürücü:**
- İndir: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
- Kur ve bilgisayarı yeniden başlat

**CH340 Sürücü:**
- İndir: https://sparks.gogo.co.nz/ch340.html
- Kur ve yeniden başlat

---

### 📧 ADIM 1.6: Veysel'e Gönderilecek Bilgiler (MÜŞTERİ)

Müşteri şu mesajı gönderecek:

```
Veysel Bey,

Hazırlıkları tamamladım:

✅ RustDesk kuruldu
   - Desktop ID: 123 456 789
   - Password: vakitmatik2026
   
✅ Python kuruldu (3.12.1)
✅ PlatformIO kuruldu
✅ ESP32-S3 USB'de (COM3)
✅ Donanımlar geldi

Test için hazırım. Ne zaman bağlanacaksınız?
```

---

## 2️⃣ VEYSEL'İN HAZIRLIK SÜRECİ

### ADIM 2.1: RustDesk Kurulumu (VEYSEL)

#### 2.1.1 İndirme ve Kurulum
```
1. Git: https://rustdesk.com/
2. Windows için indir
3. Kur (Install for all users)
4. Aç
```

#### 2.1.2 İlk Bağlantı Testi
```
Müşteriden aldığın bilgilerle:

RustDesk penceresinde:
┌────────────────────────────────┐
│  Connect to:                   │
│  ┌────────────────────┐        │
│  │  123 456 789       │ ◄──── MÜŞTERİNİN ID'sini yaz
│  └────────────────────┘        │
│         [Connect]              │ ◄──── TIK AT
└────────────────────────────────┘

Şifre penceresi açılacak:
┌────────────────────────────────┐
│  Password:                     │
│  ┌────────────────────┐        │
│  │  vakitmatik2026    │ ◄──── MÜŞTERİNİN ŞİFRESİNİ YAZ
│  └────────────────────┘        │
│         [OK]                   │ ◄──── TIK AT
└────────────────────────────────┘
```

**✅ Başarılı:** Müşterinin masaüstünü görüyorsun!  
**❌ Hata:** ID veya şifre yanlış / Müşteri RustDesk kapatmış

---

### ADIM 2.2: Kod Hazırlığı (VEYSEL)

#### 2.2.1 Local Depoda Çalış
```bash
cd "C:\Users\victus\Desktop\Kişisel Projeler\Bionluk Projeleri\Umutcan Yılmaz\ESP32_P4_Matrix_Lib"

# Güncel kodu çek (GitHub'dan)
git pull

# Değişiklikleri yap
code src/main.cpp  # VS Code ile düzenle

# Derle ve test et (local)
pio run

# Derleme başarılıysa commit
git add .
git commit -m "feat: İlk test pattern eklendi"
git push
```

#### 2.2.2 Test Senaryosu Hazırla
```
Bugün test edilecekler:
1. ✅ ESP32 USB bağlantısı
2. ✅ Kod derleme
3. ✅ Firmware yükleme
4. ✅ Serial monitor logları
5. ✅ LED panellere test pattern
```

---

### ADIM 2.3: Müşteri ile İletişim (VEYSEL)

#### 2.3.1 Randevu Mesajı
```
Umutcan Bey,

Test için hazırım. Bugün saat 14:00'te uygun musunuz?

Yapacaklarımız:
- RustDesk ile bağlanacağım
- ESP32'ye test kodunu yükleyeceğim
- LED panelleri test edeceğiz
- Fotoğraf çekeceğiz (hangi renk nerede)

Süre: ~30-60 dakika

Hazırlıklar:
- ESP32'yi USB'ye takın
- RustDesk'i açın (ID/şifre göz önünde)
- Bu süre bilgisayarı kullanmayacağız

Lütfen onaylayın.

Saygılarımla,
Veysel
```

---

## 3️⃣ İLK BAĞLANTI TESTİ

### ADIM 3.1: Bağlantı Kurulumu (VEYSEL)

#### 3.1.1 Müşteri ile Sesli Görüşme
```
WhatsApp veya telefon ile ara:
"Merhaba, şimdi bağlanıyorum. RustDesk açık mı?"
```

#### 3.1.2 RustDesk Bağlan
```
1. RustDesk aç
2. Müşteri ID'sini gir: 123 456 789
3. "Connect" butonuna bas
4. Şifreyi gir: vakitmatik2026
5. "OK"
```

#### 3.1.3 İlk İzinler
Müşterinin ekranında açılır pencere:
```
"Veysel masaüstünü görmek istiyor. İzin veriyor musunuz?"
[Accept] [Deny]
```

**Müşteriye söyle:** "Accept / Kabul Et butonuna bas"

#### 3.1.4 Kontrol Testi
```
Sen şimdi şunları görebiliyor olmalısın:
- Müşterinin Windows masaüstü
- Saat, tarih
- Açık pencereler

Mouse'u hareket ettir → Müşteri tarafında da hareket ediyorsa ✅ TAMAM
```

---

### ADIM 3.2: İlk Sistem Kontrolü (VEYSEL - Uzaktan)

#### 3.2.1 PowerShell Aç
```
Müşterinin PC'sinde (sen kontrol ediyorsun):
1. Windows tuşuna bas
2. Yaz: "powershell"
3. Enter
```

#### 3.2.2 Python Kontrol
```bash
python --version
```

Beklenen çıktı:
```
Python 3.12.x
```

✅ Görünüyorsa devam  
❌ Görmüyorsa Python'u yeniden kur

#### 3.2.3 PlatformIO Kontrol
```bash
pio --version
```

Beklenen çıktı:
```
PlatformIO Core, version 6.x.x
```

✅ Görünüyorsa devam  
❌ Görmüyorsa: `pip install platformio`

#### 3.2.4 COM Port Kontrol
```bash
pio device list
```

Beklenen çıktı:
```
COM3
----
Hardware ID: USB VID:PID=303A:1001
Description: USB JTAG/serial debug unit (COM3)
```

✅ Görünüyorsa ESP32 bağlı!  
❌ Görmüyorsa USB kablosunu kontrol et

---

## 4️⃣ YAZILIM KURULUMLARI

### ADIM 4.1: Proje Klasörü Oluştur (VEYSEL - Uzaktan)

#### 4.1.1 Masaüstünde Klasör
```bash
cd Desktop
mkdir Vakitmatik
cd Vakitmatik
```

#### 4.1.2 Git ile Projeyi İndir
```bash
git clone https://github.com/veyselka/Vakitmatik-P4-LED.git .
```

**Çıktı:**
```
Cloning into '.'...
remote: Enumerating objects: 30, done.
remote: Counting objects: 100% (30/30), done.
Receiving objects: 100% (30/30), done.
```

✅ Proje indirildi!

#### 4.1.3 Proje Yapısını Kontrol
```bash
dir
```

Görmemiz gerekenler:
```
src/
docs/
platformio.ini
README.md
```

---

### ADIM 4.2: İlk Derleme Testi (VEYSEL - Uzaktan)

#### 4.2.1 Derleme Başlat
```bash
pio run
```

**İlk çalıştırmada şunlar olacak:**
```
Platform Manager: Installing espressif32
Downloading  [####################################]  100%
Tool Manager: Installing toolchain-xtensa-esp32s3
Downloading  [####################################]  100%
...
Compiling .pio/build/esp32-s3-devkitc-1/src/main.cpp.o
...
Linking .pio/build/esp32-s3-devkitc-1/firmware.elf
Building .pio/build/esp32-s3-devkitc-1/firmware.bin
```

**Süre:** 3-5 dakika (ilk seferinde)

#### 4.2.2 Başarı Kontrolü
```
========================= [SUCCESS] Took X.XX seconds =========================
```

✅ Bu mesajı görüyorsan BAŞARILI!

#### 4.2.3 Firmware Dosyasını Kontrol
```bash
dir .pio\build\esp32-s3-devkitc-1\
```

Görmemiz gereken:
```
firmware.bin  ◄──── BU DOSYA OLMALI
firmware.elf
```

---

## 5️⃣ İLK KOD YÜKLEME

### ADIM 5.1: ESP32 Hazırlığı (VEYSEL - Uzaktan)

#### 5.1.1 USB Bağlantısı Kontrol
```bash
# COM port tekrar kontrol
pio device list
```

Çıktı:
```
COM3  ◄──── PORT NUMARASINI NOT ET
```

#### 5.1.2 Upload İşlemi
```bash
pio run --target upload
```

**Ekranda göreceksin:**
```
Configuring upload protocol...
AVAILABLE: cmsis-dap, esp-bridge, esp-builtin, esptool, espota, iot-bus-jtag, jlink, minimodule, olimex-arm-usb-ocd, olimex-arm-usb-ocd-h, olimex-arm-usb-tiny-h, olimex-jtag-tiny, tumpa
CURRENT: esptool
Looking for upload port...
Auto-detected: COM3
Uploading .pio/build/esp32-s3-devkitc-1/firmware.bin
esptool.py v4.5.1
Serial port COM3
Connecting....
Chip is ESP32-S3 (revision v0.1)
Features: WiFi, BLE
Crystal is 40MHz
MAC: 34:85:18:aa:bb:cc
Uploading stub...
Running stub...
Stub running...
Configuring flash size...
Flash will be erased from 0x00010000 to 0x000xxxxx...
Compressed 123456 bytes to 67890...
Writing at 0x00010000... (100 %)
Wrote 123456 bytes (67890 compressed) at 0x00010000 in 2.3 seconds
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
========================= [SUCCESS] Took 5.67 seconds =========================
```

✅ **SUCCESS** görüyorsan KOD YÜKLENDİ!

---

### ADIM 5.2: Serial Monitor ile Test (VEYSEL - Uzaktan)

#### 5.2.1 Serial Monitor Aç
```bash
pio device monitor --baud 115200
```

**Ekranda göreceksin:**
```
========================================
  VAKITMATIK P4 LED PANEL - İLK TEST
========================================

✓ PSRAM bulundu
Pin Konfigürasyonu:
  R1=25 G1=26 B1=27
  R2=14 G2=12 B2=13
  A=23 B=22 C=5 D=17 E=-1

Matrix başlatılıyor...
✓ Matrix başarıyla başlatıldı!
  Çözünürlük: 240x240 piksel
  Toplam Panel: 18 (3x6)

========================================
  TESTLER BAŞLIYOR...
========================================

TEST 1/5: RGB Renk Testi
  → Tüm ekran KIRMIZI olmalı
```

✅ Bu logları görüyorsan **ESP32 ÇALIŞIYOR!**

#### 5.2.2 Müşteriden Fotoğraf İste

**Sesli söyle:**
```
"Umutcan Bey, şimdi LED panellere bakın.
 Tüm ekran KIRMIZI yanıyor mu?
 Telefonunuzla fotoğraf çeker misiniz?"
```

**Müşteri fotoğrafı WhatsApp'tan gönderecek.**

---

### ADIM 5.3: Test Pattern Analizi (VEYSEL)

#### 5.3.1 Beklenen Görüntü

**TEST 1: Kırmızı Ekran**
```
Tüm 18 panel kırmızı olmalı.
Herhangi bir panel sönükse → güç kaynağı sorunu
```

**TEST 2: Yeşil Ekran**
```
2 saniye sonra tüm ekran yeşil olacak.
Eğer farklı renk görünüyorsa → pin mapping yanlış
```

**TEST 3: Mavi Ekran**
```
2 saniye sonra tüm ekran mavi.
```

**TEST 4: Grid Pattern**
```
20 piksel aralıklı yeşil-mavi çizgiler.
Eğer çizgiler bükülüyorsa → mapping sorunu
```

**TEST 5: Köşe Pikselleri**
```
4 köşede renkli noktalar:
- Sol üst: Kırmızı
- Sağ üst: Yeşil
- Sol alt: Mavi
- Sağ alt: Sarı

Eğer yanlış köşedeyse → koordinat mapping hatası
```

#### 5.3.2 Sorun Varsa

**Renkler Yanlış (Kırmızı yerine Yeşil):**
→ Pin mapping düzelt:
```cpp
// platformio.ini'de swap et
#define R1_PIN 26  // eski 25
#define G1_PIN 25  // eski 26
```

**Ekran Bölünmüş / Kaymış:**
→ 1/10 scan mapping algoritması yanlış
→ `mapCoordinates()` fonksiyonunu düzelt

**Bazı Paneller Sönük:**
→ Güç kaynağı sorunu
→ Panel bağlantılarını kontrol ettir

---

## 6️⃣ TEST SENARYOLARI

### SENARYO 1: İlk Gün Temel Test

#### 6.1.1 Amaç
- ESP32 çalışıyor mu?
- Paneller ışık veriyor mu?
- Renkler doğru mu?

#### 6.1.2 Adımlar
```
1. pio run --target upload           (kod yükle)
2. pio device monitor                 (logları gör)
3. Müşteriden fotoğraf al            (5 test pattern)
4. Analiz et
5. Sorun varsa kod düzelt
6. Tekrar upload
```

#### 6.1.3 Başarı Kriterleri
- [✅] Tüm paneller ışık veriyor
- [✅] RGB renkler doğru
- [✅] Titremiyor
- [✅] Grid düz

---

### SENARYO 2: Mapping Düzeltme

#### 6.2.1 Amaç
1/10 scan folded matrix mapping algoritmasını optimize et

#### 6.2.2 Adımlar
```
ITERASYON 1:
1. Kod değiştir (local):
   - src/ESP32_P4_Matrix.cpp mapCoordinates()
2. Commit: git commit -am "fix: mapping v1"
3. Uzaktan bağlan
4. git pull
5. pio run --target upload
6. Fotoğraf al
7. Analiz et

ITERASYON 2:
(sorun varsa tekrar)
```

#### 6.2.3 Test Noktaları
```
Koordinat  Beklenen Görüntü
─────────  ────────────────
(0, 0)     Sol üst köşe
(239, 0)   Sağ üst köşe
(0, 239)   Sol alt köşe
(239, 239) Sağ alt köşe
(120, 120) Tam merkez
```

---

### SENARYO 3: Türkçe Font Test

#### 6.3.1 Amaç
Türkçe karakterlerin doğru görüntülendiğini doğrula

#### 6.3.2 Test Metni
```cpp
dma_display->drawString(10, 10, "İMSAK");
dma_display->drawString(10, 30, "GÜNEŞ");
dma_display->drawString(10, 50, "ÖĞLE");
dma_display->drawString(10, 70, "İKİNDİ");
dma_display->drawString(10, 90, "AKŞAM");
dma_display->drawString(10, 110, "YATSI");
```

#### 6.3.3 Kontrol
- İ harfi doğru mu? (nokta üstte)
- Ğ harfi doğru mu?
- Ü, Ö harfleri doğru mu?
- Ş harfi doğru mu?

---

### SENARYO 4: Vakıt Ekranı Test

#### 6.4.1 Amaç
Tam layout testi

#### 6.4.2 Beklenen Görüntü
```
┌─────────────────────────┐
│   12:45    25 Şubat     │
├─────────────────────────┤
│ İMSAK    ●  05:30       │
│ GÜNEŞ       07:15       │
│ ÖĞLE        12:45       │
│ İKİNDİ      15:30       │
│ AKŞAM       18:00       │
│ YATSI       19:45       │
└─────────────────────────┘
```

#### 6.4.3 Kontrol Listesi
- [ ] Saat görünüyor
- [ ] Tarih görünüyor
- [ ] 6 vakit görünüyor
- [ ] Türkçe karakterler doğru
- [ ] Aktif vakit vurgulanmış (●)
- [ ] Hizalama düzgün

---

## 7️⃣ SORUN GİDERME

### SORUN 1: RustDesk Bağlanamıyor

**Semptom:** "Connection failed" hatası

**Çözüm 1:** Müşteri ID/Şifre kontrol
```
- ID doğru kopyalandı mı? (boşluklar dahil)
- Şifre büyük/küçük harf duyarlı
```

**Çözüm 2:** Güvenlik Duvarı
```
Windows Defender Firewall:
1. Kontrol Paneli → Güvenlik Duvarı
2. "Allow an app through firewall"
3. RustDesk'i bul ve işaretle
4. OK
```

**Çözüm 3:** Müşteri RustDesk Kapatmış
```
Müşteriye sesli de:
"RustDesk programı açık mı? Sistem tepsisinde (saat yanında) 
 mavi bir ikon görmeli ve 'Ready' yazmalı"
```

---

### SORUN 2: COM Port Bulunamıyor

**Semptom:**
```
Error: Please specify `upload_port` for environment
```

**Çözüm 1:** USB Kablosu Kontrol
```
Müşteriye söyle:
"USB kablosunu çıkarıp tekrar takın.
 Windows 'ding' sesi duyuyor musunuz?"
```

**Çözüm 2:** Sürücü Kurulumu
```bash
# Aygıt Yöneticisi aç
devmgmt.msc

# "Ports (COM & LPT)" altında bakılacak:
✅ "Silicon Labs CP210x USB to UART Bridge (COM3)"
❌ "Unknown Device" (sürücü yok)

Sürücü yoksa:
https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
```

**Çözüm 3:** Manuel Port Belirtme
```bash
pio run --target upload --upload-port COM3
```

---

### SORUN 3: Upload Sırasında "Timed out waiting"

**Semptom:**
```
A fatal error occurred: Failed to connect to ESP32-S3
Timed out waiting for packet header
```

**Çözüm 1:** ESP32'yi BOOT Moduna Al
```
Müşteriye talimat ver:
1. ESP32'deki "BOOT" butonuna bas ve tut
2. "RST" (reset) butonuna bir kez bas
3. "BOOT" butonunu bırak
4. Tekrar upload komutu çalıştır
```

**Çözüm 2:** USB Port Değiştir
```
Müşteriye:
"USB kablosunu bilgisayarın farklı bir portuna takın"
```

---

### SORUN 4: Paneller Işık Vermiyor

**Semptom:** Serial monitor çalışıyor ama LED'ler sönük

**Çözüm 1:** Güç Kaynağı Kontrol
```
Müşteriye:
"5V güç kaynağı açık mı?
 Yeşil LED yanıyor mu?
 Panellerin arkasındaki kırmızı LED'ler yanıyor mu?"
```

**Çözüm 2:** Panel Bağlantıları
```
Data kablosu ESP32'den ilk panele bağlı mı?
Panel-panel arası data kablosu doğru mu?
```

**Çözüm 3:** Pin Ayarları
```cpp
// platformio.ini'de pinleri kontrol et
#define R1_PIN 25
#define G1_PIN 26
...

// Müşteriye panelin arkasını görüntüle
// Pin etiketleri ile karşılaştır
```

---

### SORUN 5: Renkler Yanlış

**Semptom:** Kırmızı yerine yeşil, mavi yerine kırmızı

**Çözüm:** Pin Swapping
```cpp
// platformio.ini düzenle
// Eski:
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27

// Yeni (R ile G takas):
#define R1_PIN 26
#define G1_PIN 25
#define B1_PIN 27

// Kaydet, git commit, git push
// Uzaktan: git pull, pio run --target upload
```

**Test:** 6 farklı kombinasyon dene (RGB, RBG, GRB, GBR, BRG, BGR)

---

### SORUN 6: Ekran Bölünmüş / Kaymış

**Semptom:** Görüntü ortadan ikiye bölünmüş veya zigzag

**Neden:** 1/10 scan mapping algoritması yanlış

**Çözüm:** mapCoordinates() fonksiyonunu düzelt

```cpp
// src/ESP32_P4_Matrix.cpp içinde:

void ESP32_P4_Matrix::mapCoordinates(int x, int y, int* phys_x, int* phys_y) {
    // 1/10 scan için özel hesaplama
    // Bu algoritma panel fotoğraflarına göre ayarlanacak
    
    // ŞU AN: Basit mapping (düz geçiş)
    *phys_x = x;
    *phys_y = y;
    
    // GEREKLİ: Folded matrix için hesaplama
    // (müşteri fotoğraflarına göre iterasyon gerekiyor)
}
```

**İterasyon:** 3-5 deneme gerekebilir

---

### SORUN 7: Kod Değişiklikleri Uygulanmıyor

**Semptom:** Kodu değiştirdin ama ekranda eskisi görünüyor

**Çözüm 1:** Clean Build
```bash
pio run --target clean
pio run --target upload
```

**Çözüm 2:** Cache Temizle
```bash
rm -r .pio/build
pio run
```

**Çözüm 3:** Git Güncel mi Kontrol
```bash
# Local (senin PC'nde):
git log -1

# Remote (müşteri PC'sinde):
git log -1

# Aynı commit hash'i görmeli
# Farklıysa:
git pull
```

---

## 📋 HIZLI KOMUT REFERANSI

### RustDesk Komutları
```bash
# Bağlan
RustDesk → Connect to → [ID] → [Password]

# Dosya Gönder
Sağ üst menü → File Transfer → [Select File] → Send
```

### PlatformIO Komutları
```bash
# Derleme
pio run

# Yükleme
pio run --target upload

# Serial Monitor
pio device monitor --baud 115200

# Port Listesi
pio device list

# Temizleme
pio run --target clean

# Port Belirtme
pio run --target upload --upload-port COM3
```

### Git Komutları
```bash
# Güncelle
git pull

# Değişiklikleri Gör
git status

# Commit
git add .
git commit -m "mesaj"

# Push
git push

# Son Commit
git log -1
```

---

## ✅ BAŞARI ÖLÇÜTLERİ

### GÜN 1: Bağlantı Başarılı
- [✅] RustDesk bağlandı
- [✅] Kod derlendi
- [✅] ESP32'ye yüklendi
- [✅] Serial monitor çalışıyor

### GÜN 2: İlk Görüntü
- [✅] Paneller ışık veriyor
- [✅] RGB renkler görünüyor
- [✅] Fotoğraf alındı

### GÜN 3: Mapping Doğru
- [✅] Grid düzgün
- [✅] Köşe pikselleri doğru
- [✅] Titreme yok

### GÜN 4: Türkçe Font
- [✅] İ, Ğ, Ü, Ş, Ö, Ç görünüyor
- [✅] Vakıt isimleri doğru

### GÜN 5: Teslim
- [✅] Tam layout çalışıyor
- [✅] Video kanıtı alındı
- [✅] Dokümantasyon tamamlandı

---

## 📞 DESTEK

**Sorun yaşarsan:**
1. Bu rehbere tekrar bak
2. Sorun Giderme bölümünü kontrol et
3. Müşteri ile sesli görüş (WhatsApp/telefon)
4. Ekran görüntüsü/video iste
5. Gerekirse başka gün için planla

**İletişim:**
- GitHub Issues: https://github.com/veyselka/Vakitmatik-P4-LED/issues
- E-posta: [email]
- WhatsApp: [telefon]

---

**Hazırlayan:** Veysel Karani Kılıçerkan  
**Versiyon:** 1.0  
**Tarih:** 25 Şubat 2026  
**Proje:** Vakitmatik P4 LED Panel

**NOT:** Bu rehberi müşteriye göndermeden önce gereksiz teknik detayları çıkar. Müşteri versiyonu daha basit olmalı.
