#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include "const.h"

// Einkommentieren, sobald das neue 3.5" Waveshare-Display angeschlossen ist:
// #define USE_WAVESHARE_35_ST7796

class LGFX : public lgfx::LGFX_Device {
#ifdef USE_WAVESHARE_35_ST7796
  lgfx::Panel_ST7796 _panel_instance;
#else
  lgfx::Panel_ILI9341 _panel_instance;  // Dein aktuelles 2.4" Display
#endif
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;

 public:
  LGFX(void) {
    {  // Dein bestehender HSPI-Bus (bleibt für beide Displays 100% gleich!)
      auto cfg = _bus_instance.config();
      cfg.spi_host = HSPI_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;  // 40 MHz SPI-Takt
      cfg.freq_read = 16000000;
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = TFT_CLK;  // CLK
      cfg.pin_mosi = TFT_MOSI;  // DIN
      cfg.pin_miso = TFT_MISO;
      cfg.pin_dc = TFT_DC;  // DC
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {  // Panel-Einstellungen (automatisch passend zum gewählten Display)
      auto cfg = _panel_instance.config();
      cfg.pin_cs = TFT_CS;  // CS
      cfg.pin_rst = TFT_RST;  // RST
      cfg.pin_busy = -1;
      cfg.readable = false;

#ifdef USE_WAVESHARE_35_ST7796
      cfg.panel_width = 320;
      cfg.panel_height = 480;
      cfg.invert = true;  // IPS-Panels brauchen meistens invert = true
      cfg.rgb_order = false;
#else
      cfg.panel_width = 240;
      cfg.panel_height = 320;
      cfg.invert = false;  // Falls Farben invertiert sind, auf true ändern
      cfg.rgb_order = false;
#endif
      _panel_instance.config(cfg);
    }

    {  // Hintergrundbeleuchtung (BL auf GPIO 17)
      auto cfg = _light_instance.config();
      cfg.pin_bl = TFT_BL;
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance);
  }
};