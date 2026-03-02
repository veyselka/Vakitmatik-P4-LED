# 🔧 80x40 LED Panel Test Kodu - Kullanım Kılavuzu

## ⚙️ ADIM 1: Arduino IDE Kurulumu

1. **Arduino IDE 2.x** indirin: https://www.arduino.cc/en/software
2. Arduino IDE'yi açın

---

## 📚 ADIM 2: ESP32 Board Desteği Ekleyin

1. Arduino IDE'de: **Dosya → Tercihler**
2. "Ek Kart Yöneticisi URL'leri" kutusuna ekleyin:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. **Araçlar → Kart → Kart Yöneticisi**
4. "esp32" arayın ve **"ESP32 by Espressif Systems"** kurun (v3.x)

---

## 📖 ADIM 3: Gerekli Kütüphaneleri Yükleyin

**Araçlar → Kütüphane Yöneticisi** (veya Ctrl+Shift+I)

Sırayla şunları arayıp **KURUN**:

1. **ESP32 HUB75 LED MATRIX PANEL DMA Display** (by mrfaptastic) - v3.x
2. **Adafruit GFX Library** - v1.11.x veya üstü

---

## 🔌 ADIM 4: PIN BAĞLANTILARI KONTROLÜ (ÖNEMLİ!)

### 📌 Mevcut Pin Tanımları (27. satırdan itibaren):

```cpp
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
#define E_PIN   -1    // Kullanılmıyor
#define LAT_PIN  4
#define OE_PIN  15
#define CLK_PIN 16
```

### ⚠️ SİZİN DONANIM BAĞLANTINIZA GÖRE DEĞİŞTİRİN!

**Nasıl öğrenirim?**
- Eğer daha önce başka bir kod çalıştırdıysanız, o koddaki pin tanımlarını kullanın
- Veya: ESP32 + HUB75 panel arasında adaptör board varsa, üzerindeki pin etiketlerine bakın
- Ya da: Fiziksel kablo bağlantılarını takip edin (hangi ESP32 pini hangi HUB75 sinyaline gidiyor)

### 🔧 Değiştirme Örneği:
Mesela sizin R1 pini GPIO 25'e bağlıysa:
```cpp
#define R1_PIN   25  // 8 yerine 25 yazdık
```

---

## 🖥️ ADIM 5: Board ve Port Ayarları

1. **Araçlar → Kart** → **"ESP32S3 Dev Module"** seçin
2. **Araçlar → PSRAM** → **"OPI PSRAM"** seçin (ÖNEMLİ!)
3. **Araçlar → USB CDC On Boot** → **"Enabled"**
4. **Araçlar → Port** → ESP32'nizin bağlı olduğu COM portunu seçin (örn: COM3, COM7 vs)

---

## ⬆️ ADIM 6: Kodu Yükleyin

1. **01_ScanDiagnostic.ino** dosyasını Arduino IDE'de açın
2. Sol üst köşedeki **✓ (Doğrula)** butonuna tıklayın
   - Hata yoksa "Derleme tamamlandı" mesajı gelir
   - **Hata varsa:** Pin tanımlarını veya kütüphaneleri kontrol edin
3. **→ (Yükle)** butonuna tıklayın
4. Yükleme bitince "Yükleme tamamlandı" mesajı gelir

---

## 📊 ADIM 7: Seri Monitörü Açın ve Test Edin

1. **Araçlar → Seri Monitör** (veya Ctrl+Shift+M)
2. Sağ altta **115200 baud** seçin
3. Ekranda şu mesajları göreceksiniz:

```
==============================================
  01_ScanDiagnostic  |  ESP32-S3 P4 80x40
==============================================
  PSRAM: xxxxx bytes
  Heap: xxxxx bytes
  
  Panel OK - 80x40 (tek parça, 1/10 scan)
  Driver: ICN2037BP | E-pin: KULLANILMIYOR (OK)
  
=== TESTLER BAŞLIYOR ===
```

---

## 🧪 TEST SONUÇLARI NASIL OKUNUR?

### Test 0-A: Anahtar Satır Testi
- **Her 2.5 saniyede** farklı renkli tek bir yatay çizgi görmelisiniz
- Seri monitörde "Buf 0", "Buf 1", "Buf 2" vs yazacak
- **KAÇ TANE YATAY ÇİZGİ GÖRDÜĞÜNÜZÜ SAYIP NOT EDİN!**

### Test 0-B: Satır Çift Karşılaştırma
- 2-4 renkli yatay bantlar göreceksiniz
- **Kaç farklı renk gördüğünüzü not edin**

### Test 1: Solid Renkler
- Tüm panel sırayla: KIRMIZI → YEŞİL → MAVİ → BEYAZ olmalı
- ✅ **Doğruysa:** Pin bağlantıları OK
- ❌ **Yanlış/Karışık renkler:** R1/G1/B1 pinleri yanlış

### Test 2: Yatay Bantlar
- Üst yarı KIRMIZI, alt yarı YEŞİL olmalı
- ❌ **Tekrar ediyorsa:** Scan mapping sorunu var

### Test 3: Çerçeve
- 80x40 boyutunda dikdörtgen çerçeve
- 4 köşede farklı renkli noktalar

### Test 4: Row Scan
- Tek yeşil çizgi yukarıdan aşağıya kaymalı (40 satır)

### Test 5: Büyük L Harfi
- Tüm panelden tek bir düzgün "L" harfi görmelisiniz

---

## ❌ SORUN GİDERME

### "PSRAM bulunamadı" hatası:
→ **Araçlar → PSRAM → OPI PSRAM** seçin

### "Panel baslatamadi!" hatası:
→ Pin tanımlarını kontrol edin (27-40. satırlar)

### Panel hiç yanmıyor:
1. Güç kablosu takılı mı? (HUB75 panellerin ayrı 5V güç girişi var)
2. Pin bağlantıları doğru mu?

### Rastgele ışıklar / Tekrar eden desenler:
→ **PIN BAĞLANTILARI YANLIŞ!** Her pini tek tek kontrol edin

### Renkler yanlış (mavi yerine kırmızı vs):
→ R1/G1/B1/R2/G2/B2 pinlerini karıştırmışsınız

---

## 📸 TEST SONUÇLARINI GÖNDERİN

Lütfen şunları fotoğraflayıp gönderin:
1. **Test 0-A** sırasında kaç satır yandığını (örnek: "2 satır yan yana", "4 satır aralıklı")
2. **Test 1** (Solid renkler) - Tüm panel tek renk mı oluyor?
3. **Test 5** (L harfi) - L düzgün mü yoksa bölünmüş/tekrarlanmış mı?
4. **Seri monitör çıktısının tamamını** (kopyala-yapıştır)

---

## 📞 DESTEK

Sorun yaşarsanız şunları gönderin:
- Fotoğraflar (panel + seri monitör)
- Hangi adımda takıldınız?
- Hata mesajları varsa tam metni

**Başarılar! 🚀**
