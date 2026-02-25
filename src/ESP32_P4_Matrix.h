/**
 * ESP32_P4_Matrix.h
 * 
 * ESP32-S3 için 240x240 P4 LED Panel sürücüsü
 * HUB75 DMA kullanarak 18 panel kontrolü
 * 
 * Özellikler:
 * - 3x6 panel dizilimi (240x240 toplam)
 * - 1/10 Scan Folded Matrix mapping
 * - PSRAM frame buffer
 * - Türkçe karakter desteği
 * 
 * @author Veysel Karani Kılıçerkan
 */

#ifndef ESP32_P4_MATRIX_H
#define ESP32_P4_MATRIX_H

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Panel konfigürasyonu (platformio.ini'den gelecek)
#ifndef PANEL_WIDTH
    #define PANEL_WIDTH 80
#endif

#ifndef PANEL_HEIGHT
    #define PANEL_HEIGHT 40
#endif

#ifndef PANELS_X
    #define PANELS_X 3
#endif

#ifndef PANELS_Y
    #define PANELS_Y 6
#endif

#define TOTAL_WIDTH (PANEL_WIDTH * PANELS_X)   // 240
#define TOTAL_HEIGHT (PANEL_HEIGHT * PANELS_Y) // 240

// Renk tanımlamaları (RGB565 format)
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F

class ESP32_P4_Matrix {
public:
    ESP32_P4_Matrix();
    ~ESP32_P4_Matrix();
    
    /**
     * Matrix'i başlat
     * @return true başarılı, false hata
     */
    bool begin();
    
    /**
     * Ekranı temizle
     * @param color Arkaplan rengi (varsayılan: siyah)
     */
    void clear(uint16_t color = COLOR_BLACK);
    
    /**
     * Test pattern göster (grid)
     */
    void testPattern();
    
    /**
     * Parlaklık ayarla
     * @param brightness 0-255 arası değer
     */
    void setBrightness(uint8_t brightness);
    
    /**
     * Pixel çiz
     * @param x X koordinatı (0-239)
     * @param y Y koordinatı (0-239)
     * @param color RGB565 renk
     */
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    
    /**
     * Çizgi çiz
     */
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    
    /**
     * Dikdörtgen çiz
     */
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    
    /**
     * Dolu dikdörtgen çiz
     */
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    
    /**
     * Metin yaz (basit, Türkçe desteği için TurkishFont kullanılacak)
     */
    void drawText(const char* text, int16_t x, int16_t y, uint16_t color);
    
    /**
     * DMA display nesnesine erişim (ileri seviye kullanım)
     */
    MatrixPanel_I2S_DMA* getDisplay() { return dma_display; }
    
private:
    MatrixPanel_I2S_DMA* dma_display;
    
    /**
     * HUB75 pinlerini ayarla
     */
    void setupPins(HUB75_I2S_CFG::i2s_pins& pins);
    
    /**
     * 1/10 Scan mapping (şimdilik basit, TASK-004'te geliştirilecek)
     */
    void mapCoordinates(int16_t& x, int16_t& y);
};

#endif // ESP32_P4_MATRIX_H
