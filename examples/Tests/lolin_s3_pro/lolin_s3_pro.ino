// logging to the same Serial interface whatever the configuration
#if defined ARDUINO_HW_CDC_ON_BOOT
  #define LOG log_n
#else
  #define LOG Serial.println();Serial.printf
#endif

#include <SD.h>
#define LGFX_USE_V1
#define LGFX_AUTODETECT
// Select board "LOLIN S3"
#define LGFX_LOLIN_S3_PRO
#include <LovyanGFX.hpp>

static LGFX tft;

// SD sharing bus with TFT/Touch
#define TFCARD_CS_PIN 46 // LoLin S3Pro specific
#define SD_busCfg ((lgfx::v1::Bus_SPI*)tft.getPanel()->bus())->config() // shorthand to panel bus config
#define SD_initBus() SD_SPI->begin( SD_busCfg.pin_sclk, SD_busCfg.pin_miso, SD_busCfg.pin_mosi, TFCARD_CS_PIN )
#define SD_releaseBus() SD_SPI->end()
SPIClass *SD_SPI = nullptr;
void SD_PreInit();
void testSD();
void printSD( bool recursive = false );
void printDirectory(File dir, int numTabs, bool recursive );


void SD_PreInit()
{
  SD_SPI = new SPIClass( SD_busCfg.spi_host );

  LOG("LGFX SD/TFT/Touch shared SPI%d_HOST @ %d Hz", SD_busCfg.spi_host+1, SD_busCfg.freq_write );

  SD_initBus();

  if(! SD.begin( TFCARD_CS_PIN, *SD_SPI, SD_busCfg.freq_write ) ) {
    LOG("SD FAILED, halting");
    while(1) vTaskDelay(1);
  }

  SD_releaseBus();
}


void testSD()
{
  tft.waitDisplay();
  tft.releaseBus();

  SD_initBus();
  printSD( false );
  SD_releaseBus();

  tft.initBus();
}


void printDirectory(File dir, int numTabs, bool recursive )
{
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) { // no more files
      break;
    }
    if (entry.isDirectory()) {
      LOG("[DIR] %s", entry.name());
      if( recursive ) {
        printDirectory(entry, numTabs + 1, true);
      }
    } else {
      time_t lw = entry.getLastWrite();
      struct tm * _tm = localtime(&lw);
      LOG("[FILE][%s\t%d bytes\t %d-%02d-%02d %02d:%02d:%02d", entry.name(), entry.size(), (_tm->tm_year) + 1900, (_tm->tm_mon) + 1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
    }

    SD_releaseBus();
    tft.initBus();

    tft.println( entry.name() );

    tft.waitDisplay();
    tft.releaseBus();
    SD_initBus();

    entry.close();
  }
}


void printSD( bool recursive )
{
  String cardType;
  switch (SD.cardType()) {
    case CARD_NONE: LOG("No SD Detected :("); return;
    case CARD_MMC:  cardType = "MMC";  break;
    case CARD_SD:   cardType = "SD";   break;
    case CARD_SDHC: cardType = "SDHC"; break;
    default: cardType = "Unknown";
  }
  LOG("[Card] type=%s, size=%llu KBytes, total=%llu Bytes, used=%llu Bytes", cardType.c_str(), (uint64_t)(SD.cardSize()/1000u), SD.totalBytes(), SD.usedBytes());
  File dir =  SD.open("/");
  if( dir ) {
    printDirectory(dir, 0, recursive );
    dir.close();
  }
}


LGFX_Sprite *sprite = nullptr;

void c64logo( float width=100.0 )
{
  if( !sprite ) {
    sprite = new LGFX_Sprite( &tft );
  } else {
    sprite->deleteSprite();
  }

  float height = 0.97*width; // i
  float vmid = 0.034*width;  // b
  float v1 = 0.166*width;    // c
  float v2 = 0.300*width;    // d
  float h1 = 0.364*width;    // e
  float rv = 0.520*width/2;    // f
  float rh = 0.530*width/2;    // g
  float h2 = 0.636*width;    // h

  sprite->createSprite( width, height );
  sprite->fillSprite( TFT_WHITE );
  sprite->fillEllipse( width/2, height/2, width/2, height/2, TFT_BLUE );
  sprite->fillEllipse( width/2, height/2, rh, rv, TFT_WHITE );
  sprite->fillRect( h2, v2, h1, v1+vmid+v1, TFT_BLUE ); // prepare
  sprite->fillRect( h2, 0, h1, v2, TFT_WHITE );    // crop upper white square
  sprite->fillRect( h2, v2+v1, h1, vmid, TFT_WHITE ); // crop middle stroke
  sprite->fillRect( h2, v2+v1+vmid+v1, h1, v2, TFT_WHITE ); // crop lower white square
  sprite->fillTriangle( width, v2, width, v2+v1+vmid+v1, h2+h1/2, v2+v1+vmid/2, TFT_WHITE );

  sprite->pushSprite( tft.width()/2-width/2, tft.height()/2-width/2 );
}




void setup()
{
  Serial.begin(115200);
  delay(2000);
  // Test all log functions to see which one prints in the chosen console
  Serial.println("Hello from Serial.println()\n");
  log_e("Hello from log_e()\n");
  LOG("Hello from LOG()\n");

  tft.init();
  SD_PreInit();
}



void loop()
{
  vTaskDelay(1000);
  tft.fillScreen( rand()%0xffffff );
  c64logo( (rand()%50)+50 );
  tft.setCursor(0, 0);
  testSD();
}
