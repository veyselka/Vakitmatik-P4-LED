# **VS Code / Copilot Ortamında Donanımsız Geliştirme Gereksinim Analizi**

**Proje:** Vakitmatik P4 LED Panel (ESP32-S3, 18 Panel, 1/10 Scan)

**Geliştirme Ortamı:** Visual Studio Code \+ PlatformIO \+ GitHub Copilot

## **1\. Yazılım Ortamı Kurulum Gereksinimleri**

VS Code üzerinde bu projeyi sorunsuz derleyebilmek için aşağıdaki araç zincirinin (Toolchain) eksiksiz kurulması şarttır:

* **IDE:** Visual Studio Code.  
* **Eklenti 1:** PlatformIO IDE (ESP32-S3 derleyicisi ve bağımlılık yönetimi için mecburi).  
* **Eklenti 2:** C/C++ (IntelliSense için).  
* **AI Asistan:** GitHub Copilot.

## **2\. PlatformIO Proje Yapılandırması (platformio.ini)**

Kodu VS Code'da yazarken Copilot'a sürekli rehberlik etmesi için platformio.ini dosyanız şu şekilde ayarlanmalıdır (Bu olmadan derleme hatası alırsınız):

* board \= esp32-s3-devkitc-1  
* build\_flags \= \-DBOARD\_HAS\_PSRAM (Bu satır olmadan kütüphane 240x240 pikseli desteklemez ve çöker).  
* Bağımlılıklar: mrfaptastic/ESP32 HUB75 LED MATRIX PANEL DMA Display kütüphanesi.

## **3\. Simülasyon ve Test Stratejisi (Kritik Engel)**

**Durum:** Elde ESP32-S3 kartı ve 1/10 Scan P4 panel bulunmamaktadır. HUB75 DMA protokolü Wokwi gibi çevrimiçi simülatörlerde desteklenmemektedir.

**Çözüm Planı (Remote Hardware in the Loop \- Uzaktan Donanım Testi):**

Kodun temel algoritmaları (koordinat hesaplama vb.) VS Code içinde test edilecek, ancak donanımsal sürücü tepkileri doğrudan müşterinin cihazında test edilecektir.

* **Gereksinim 1:** Müşterinin ESP32-S3 N16R8 kartını teslim alması.  
* **Gereksinim 2:** Müşterinin bilgisayarına uzak masaüstü (AnyDesk/TeamViewer) kurulması.  
* **Gereksinim 3 (İteratif Test):** Geliştirici (Siz), derlenmiş .bin dosyasını uzaktan müşterinin kartına yükleyecek ve donanımsal çıktıları (Renk kayması, satır atlama, Folded Matrix hataları) müşteriden gelecek fotoğraflarla analiz ederek kodda düzeltmeler yapacaktır.

## **4\. Kütüphane Mimarisi (Copilot için İpuçları)**

Copilot ile kütüphane iskeletini oluştururken AI'a verilmesi gereken ana komutlar (Prompts):

* "ESP32-S3 PSRAM kullanarak 240x240 frame buffer tahsis et."  
* "18 adet 80x40 paneli, 3 sütun 6 satır şeklinde zincirleme bağlayacak Custom Mapper yaz."  
* **En Önemli Prompt:** "Panel donanımsal olarak 1/10 scan, yani 160x20 fiziksel boyutta davranıyor. Kullanıcının girdiği (X, Y) koordinatlarını (0-239, 0-239), bu 160x20 Folded Matrix mantığına çeviren matematiksel formülü üret."

## **5\. Çıkabilecek Donanım Kaynaklı Hatalar (Uzaktan Çözülecekler)**

Müşteri kodu ilk yüklediğinde %99 ihtimalle karşılaşılacak hatalar ve çözüm yöntemleri:

1. **Renklerin Ters Olması (Örn: Kırmızı yerine Yeşil yanması):** platformio.ini veya kod içindeki I2S ayarlarından latch\_blanking veya RGB pin sıralaması (R1G1B1) değiştirilerek çözülür.  
2. **Yazıların Ortadan İkiye Bölünmesi/Kayması:** 1/10 Scan Folded Matrix (Katlanmış Ekran) algoritmasının (Custom Mapper) koordinat çeviri matematiğinin hatalı olduğunun kanıtıdır. Müşteriden gelen fotoğraftaki kayma miktarına (offset) bakılarak matematikte düzeltme yapılır.  
3. **Ekranın Sürekli Titremesi (Flickering):** İşlemci veya DMA frekansının yetersizliğidir. Kodda mxconfig.clkphase ayarı veya ICN2037BP'ye özel clock hızı değiştirilir.