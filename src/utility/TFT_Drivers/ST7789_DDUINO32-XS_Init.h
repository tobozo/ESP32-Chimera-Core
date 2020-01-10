log_d("D-Duino32-XS driver init");

static uint8_t PROGMEM
  cmd_240x240[] = {                         // Initialization commands for 7789 screens
    12,                                     // 9 commands in list:
    ST7789_SWRESET,    1,                   // 1: Software reset, no args
      150,                                  // 150 ms delay
    ST7789_SLPOUT,     1,                   // 2: Out of sleep mode, no args
      255,                                  // 255 = 500 ms delay
    ST7789_COLMOD,     2,                   // 3: Set color mode, 1 arg:
      0x55,                                 // 16-bit color
      10,                                   // 10 ms delay
    ST7789_MADCTL,     1,                   // 4: Memory access ctrl (directions), 1 arg:
      0x00,                                 // Row addr/col addr, bottom to top refresh
    ST7789_CASET,      4,                   // 5: Column addr set, 4 args
      0x00, ST7789_240x240_XSTART,          // XSTART = 0
    (240+ST7789_240x240_XSTART) >> 8,
    (240+ST7789_240x240_XSTART) & 0xFF,   // XEND = 240
    ST7789_RASET,      4,                   // 6: Row addr set, 4 args
      0x00, ST7789_240x240_YSTART,          // YSTART = 0
      (240+ST7789_240x240_YSTART) >> 8,
    (240+ST7789_240x240_YSTART) & 0xFF,   // YEND = 240
    ST7789_INVON,      1,                   // 7: Inversion ON
      10,
    ST7789_NORON,      1,                   // 8: Normal display on, no args
      10,                                   // 10 ms delay
    ST7789_PVGAMCTRL, 14,                   // Positive voltage gamma control, 14 args
      0xd0,0x00,0x05,0x0e,0x15,0x0d,0x37,0x43,0x47,0x09,0x15,0x12,0x16,0x19,
    ST7789_NVGAMCTRL, 14,                   // Negative voltage gamma control, 14 args
      0xd0,0x00,0x05,0x0d,0x0c,0x06,0x2d,0x44,0x40,0x0e,0x1c,0x18,0x16,0x19,
    ST7789_DISPON,     1,                   // 9: Main screen turn on, no args
    255 };                                  // 255 = 500 ms delay

  commandList( cmd_240x240 );