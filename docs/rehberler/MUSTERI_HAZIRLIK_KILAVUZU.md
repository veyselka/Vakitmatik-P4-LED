# 📱 MÜŞTERİ HAZIRLIK KILAVUZU

**Vakitmatik P4 LED Panel Projesi**  
**Müşteri:** Umutcan Yılmaz  
**Geliştirici:** Veysel Karani Kılıçerkan

---

## 🎯 AMAÇ

Bu kılavuz, yazılım testleri için bilgisayarınızı hazırlamanızı sağlayacak.

Veysel Bey, uzaktan bilgisayarınıza bağlanıp ESP32'ye kod yükleyecek ve LED panelleri test edecek.

**Toplam Süre:** 30-45 dakika

---

## ✅ YAPILACAKLAR LİSTESİ

- [ ] **1. RustDesk Kurulumu** (uzaktan erişim için)
- [ ] **2. Python Kurulumu** (kod derlemek için)
- [ ] **3. PlatformIO Kurulumu** (ESP32 programlama için)
- [ ] **4. USB Sürücü Kurulumu** (ESP32 bağlantısı için)
- [ ] **5. Test Randevusu Ayarlama**

---

## 1️⃣ RUSTDESK KURULUMU

### Ne İşe Yarar?
Veysel Bey, uzaktan bilgisayarınızı görecek ve test yapacak (TeamViewer veya AnyDesk gibi).

### Adımlar

#### 1.1 İndirme
1. Tarayıcınızı açın
2. Şu adrese gidin: **https://rustdesk.com/**
3. "Download for Windows" butonuna tıklayın
4. `rustdesk-1.x.x.exe` dosyası inecek

#### 1.2 Kurulum
1. İndirilen dosyaya çift tıklayın
2. "Evet" / "Run" butonuna basın
3. "Install for all users" seçeneğini işaretleyin
4. "Install" butonuna basın
5. Kurulum bitince program otomatik açılacak

#### 1.3 ID ve Şifre Bilgilerini Alın

RustDesk açıldığında şöyle bir ekran göreceksiniz:

```
┌────────────────────────────────┐
│  Your Desktop ID               │
│  ┌────────────────────┐        │
│  │  123 456 789       │ ◄──── BU NUMARAYI KOPYALAYIN
│  └────────────────────┘        │
│                                │
│  Password                      │
│  ┌────────────────────┐        │
│  │  abcd1234          │ ◄──── BU ŞİFREYİ KOPYALAYIN
│  └────────────────────┘        │
└────────────────────────────────┘
```

**ÖNEMLİ:** Bu 2 bilgiyi Veysel Bey'e gönderin:
- Desktop ID: `123 456 789`
- Password: `abcd1234`

**WhatsApp Mesajı Şablonu:**
```
RustDesk kuruldu.
Desktop ID: 123 456 789
Password: abcd1234
```

#### 1.4 Şifreyi Değiştirme (Önerilen - Opsiyonel)

Daha kolay hatırlanır bir şifre belirleyebilirsiniz:

1. "Password" yanındaki ⚙️ ayar simgesine tıklayın
2. "Set permanent password" seçeneğine tıklayın
3. Yeni şifrenizi girin (örn: `vakitmatik2026`)
4. "OK" butonuna basın
5. **Yeni şifreyi** Veysel Bey'e gönderin

---

## 2️⃣ PYTHON KURULUMU

### Ne İşe Yarar?
PlatformIO çalışması için gerekli.

### Adımlar

#### 2.1 İndirme
1. Tarayıcıyı açın
2. Şu adrese gidin: **https://www.python.org/downloads/**
3. "Download Python 3.x.x" (sarı büyük buton) tıklayın
4. İndirilen dosyayı çalıştırın

#### 2.2 Kurulum

**⚠️ ÇOK ÖNEMLİ - MUTLAKA OKUYUN:**

Kurulum ekranının **EN ALTINDA** şu seçeneği **MUTLAKA** işaretleyin:

```
[✓] Add Python to PATH  ◄──── BU ŞEÇENEĞİ İŞARETLEYİN!
```

Sonra:
1. "Install Now" butonuna basın
2. Kurulum bitince "Disable path length limit" yazısına tıklayın
3. "Close" butonuna basın

#### 2.3 Kontrol

Kurulum doğru yapıldı mı kontrol edin:

1. Windows tuşuna basın
2. "cmd" yazın ve Enter basın
3. Açılan siyah pencereye şunu yazın:
   ```
   python --version
   ```
4. Enter basın

**Görmemiz gereken:**
```
Python 3.12.x
```

✅ Bu mesajı görüyorsanız TAMAM!  
❌ Görmüyorsanız Python'u kaldırıp tekrar kurun (**ADD TO PATH** işaretlemeyi unutmayın!)

---

## 3️⃣ PLATFORMIO KURULUMU

### Ne İşe Yarar?
ESP32'ye kod yüklemek için gerekli.

### Adımlar

#### 3.1 Komut İstemi Açın
1. Windows tuşuna basın
2. "powershell" yazın
3. Enter basın (mavi bir pencere açılacak)

#### 3.2 Kurulum Komutunu Çalıştırın

Açılan pencereye şunu **AYNEN** yazın (kopyala-yapıştır yapabilirsiniz):

```
pip install platformio
```

Enter basın.

Ekranda bir sürü yazı akacak (30-60 saniye). Sabırla bekleyin.

**En sonda göreceksiniz:**
```
Successfully installed platformio-6.x.x
```

✅ Bu mesajı görüyorsanız TAMAM!

#### 3.3 Kontrol

Aynı pencereye şunu yazın:
```
pio --version
```

Enter basın.

**Görmemiz gereken:**
```
PlatformIO Core, version 6.x.x
```

✅ Bu mesajı görüyorsanız BAŞARILI!

---

## 4️⃣ USB SÜRÜCÜ KURULUMU

### Ne İşe Yarar?
Bilgisayar ile ESP32 arasındaki iletişim için gerekli.

### Adımlar

#### 4.1 ESP32'yi Bilgisayara Bağlayın
1. ESP32-S3 kartını USB kablosu ile bilgisayara takın
2. Windows "ding" sesi duyacaksınız (cihaz tanındı)

#### 4.2 Kontrol Edin

PowerShell'de (hala açıksa, değilse tekrar açın) şunu yazın:
```
pio device list
```

**Görmemiz gereken:**
```
COM3
----
Hardware ID: USB VID:PID=10C4:EA60
Description: Silicon Labs CP210x USB to UART Bridge (COM3)
```

✅ `COM3` (veya COM4, COM5 vb.) görüyorsanız TAMAM!

#### 4.3 Sürücü Kurulmamışsa

Eğer "Cihaz tanınmadı" hatası alıyorsanız:

**CP210x Sürücü İndirme:**
1. Şu adrese gidin: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
2. "Download for Windows" tıklayın
3. İndirilen dosyayı kurun
4. Bilgisayarı yeniden başlatın
5. Tekrar kontrol edin

---

## 5️⃣ VEYSEL BEY'E GÖNDERİLECEK BİLGİLER

Tüm kurulumlar tamamlandıktan sonra şu mesajı gönderin:

### WhatsApp/E-posta Mesajı:

```
Veysel Bey,

Tüm hazırlıklar tamamlandı:

✅ RustDesk kuruldu
   Desktop ID: 123 456 789
   Password: vakitmatik2026

✅ Python kuruldu
✅ PlatformIO kuruldu
✅ ESP32 USB'de bağlı (COM3)
✅ LED paneller bağlandı

Test için hazırım. Uygun olduğunuz bir zamanda 
bildirebilir misiniz?

Saygılarımla,
Umutcan Yılmaz
```

---

## 6️⃣ TEST GÜNÜ HAZIRLIK

### Test Saatinden Önce

1. ✅ **RustDesk'i açın** (masaüstünde simge olmalı)
2. ✅ **ESP32'yi USB'ye takın**
3. ✅ **LED panellerin güç kaynağını açın**
4. ✅ **ID ve şifreyi Veysel Bey'e tekrar gönderin** (emin olmak için)

### Test Sırasında

1. **RustDesk izin penceresi açıldığında "Kabul Et / Accept" butonuna basın**
   
   Şöyle bir pencere göreceksiniz:
   ```
   "Veysel masaüstünüzü görmek istiyor. İzin veriyor musunuz?"
   [Kabul Et] [Reddet]
   ```
   
   "Kabul Et" butonuna basın.

2. **Bilgisayarı kullanmayın**
   - Veysel Bey mouse ve klavyeyi kullanacak
   - Siz sadece izleyin
   - LED panellere bakın ve fotoğraf çekin

3. **Veysel Bey'in talimatlarını bekleyin**
   - "Ekrana bakabilir misiniz?"
   - "Hangi rengi görüyorsunuz?"
   - "Telefonunuzla fotoğraf çekebilir misiniz?"

4. **Fotoğrafları WhatsApp'tan gönderin**
   - LED panellerin tamamını görecek şekilde
   - Net olsun (odaklı, bulanık değil)
   - Her test için ayrı fotoğraf

### Test Sonrası

Veysel Bey "Test tamamlandı" dediğinde:
- RustDesk'i kapatabilirsiniz
- ESP32'yi bağlı bırakabilirsiniz (güç kesmeyin)
- Sonraki test için randevu alın

---

## ❓ SIK SORULAN SORULAR

### "RustDesk güvenli mi?"
Evet, açık kaynak bir yazılımdır. Sadece test süresince açık tutun, sonra kapatabilirsiniz.

### "Bilgisayarıma zarar verir mi?"
Hayır. Veysel Bey sadece PowerShell komutları çalıştıracak ve kod yükleyecek.

### "Test ne kadar sürer?"
İlk test 30-60 dakika, sonraki testler 15-20 dakika.

### "İnternetsiz olur mu?"
Hayır. RustDesk bağlantısı için internet gerekli.

### "Hangi saatlerde test yapılacak?"
Veysel Bey sizinle günlük randevu alacak (genellikle 14:00-18:00 arası).

### "Sorun çıkarsa ne yapmalıyım?"
Veysel Bey'i arayın. Sorun giderme için sesli konuşmak daha hızlıdır.

---

## 📞 İLETİŞİM

**Sorularınız için:**
- WhatsApp: +90 507 840 93 23
- E-posta: veyselklcrn321@gmail.com
- GitHub: https://github.com/veyselka/Vakitmatik-P4-LED

---

## ✅ KONTROL LİSTESİ

Teslimata hazır mısınız? Kontrol edin:

- [ ] RustDesk kurulu ve açılıyor
- [ ] ID ve şifre Veysel Bey'e gönderildi
- [ ] Python kurulu (`python --version` çalışıyor)
- [ ] PlatformIO kurulu (`pio --version` çalışıyor)
- [ ] ESP32 USB'de görünüyor (`pio device list` COM3'ü gösteriyor)
- [ ] LED paneller güç kaynağına bağlı
- [ ] Test randevusu alındı

**Hepsi tamam mı? MÜKEMMEL! Test için hazırsınız! 🎉**

---

**Hazırlayan:** Veysel Karani Kılıçerkan  
**Tarih:** 25 Şubat 2026  
**Proje:** Vakitmatik P4 LED Panel  

**NOT:** Bu kılavuzu yazdırıp yanınızda tutabilirsiniz.
