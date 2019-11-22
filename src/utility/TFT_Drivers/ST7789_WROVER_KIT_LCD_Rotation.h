

if( _id > 0 ){
  log_d("ST7789 rotate : %d", m);
  writecommand(TFT_MADCTL);
  rotation = m % 4;
  switch (rotation) {
    case 0: // Portrait
#ifdef CGRAM_OFFSET
      colstart = 0;
      rowstart = 0;
#endif
      writedata(TFT_MAD_COLOR_ORDER);

      _width  = _init_width;
      _height = _init_height;
      break;

    case 1: // Landscape (Portrait + 90)
#ifdef CGRAM_OFFSET
      colstart = 0;
      rowstart = 0;
#endif
      writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_COLOR_ORDER);

      _width  = _init_height;
      _height = _init_width;
      break;

    case 2: // Inverter portrait
#ifdef CGRAM_OFFSET
       colstart = 0;
       rowstart = 80;
#endif
      writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_COLOR_ORDER);

      _width  = _init_width;
      _height = _init_height;
       break;
    case 3: // Inverted landscape
#ifdef CGRAM_OFFSET
      colstart = 80;
      rowstart = 0;
#endif
      writedata(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_COLOR_ORDER);

      _width  = _init_height;
      _height = _init_width;
      break;
  }

} else {
  log_d("ILI9341 rotate : %d", m);
  // ILI9341 rotations
  typedef struct {
    uint8_t madctl;
    uint8_t bmpctl;
    uint16_t width;
    uint16_t height;
  } rotation_data_t;
  const rotation_data_t ili9341_rotations[4] = {
    {(TFT_MAD_MX|TFT_MAD_BGR),(TFT_MAD_MX|TFT_MAD_MY|TFT_MAD_BGR),TFT_WIDTH,TFT_HEIGHT},
    {(TFT_MAD_MV|TFT_MAD_BGR),(TFT_MAD_MV|TFT_MAD_MX|TFT_MAD_BGR),TFT_HEIGHT,TFT_WIDTH},
    {(TFT_MAD_MY|TFT_MAD_BGR),(TFT_MAD_BGR),TFT_WIDTH,TFT_HEIGHT},
    {(TFT_MAD_MX|TFT_MAD_MY|TFT_MAD_MV|TFT_MAD_BGR),(TFT_MAD_MY|TFT_MAD_MV|TFT_MAD_BGR),TFT_HEIGHT,TFT_WIDTH}
  };
  rotation = m % 4; // can't be higher than 3
  m = ili9341_rotations[rotation].madctl;
  _width  = ili9341_rotations[rotation].width;
  _height = ili9341_rotations[rotation].height;

  writecommand(TFT_MADCTL);
  writedata(m);

}
