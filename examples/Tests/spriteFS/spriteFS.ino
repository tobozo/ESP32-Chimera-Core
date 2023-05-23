#include <ESP32-Chimera-Core.h>

struct SpriteFS_t
{
  uint16_t width;
  uint16_t height;
  uint32_t buffer_length;
};


class SpriteFS
{
  public:
    SpriteFS( LGFX_Sprite* sptr ) : sprite( sptr ) { assert( sprite ); }
    LGFX_Sprite* sprite;
    uint32_t fsduration = 0;
    uint32_t streamduration = 0;

    bool save( fs::FS* fs, const char* filename )
    {
      uint32_t start = millis();
      bool ret = false;
      fs::File outFile = fs->open( filename, "w" );
      if( outFile ) {
        ret = save( (Stream*)&outFile );
        outFile.close();
      }
      fsduration = millis() - start;
      return ret;
    }

    bool load( fs::FS* fs, const char* filename )
    {
      uint32_t start = millis();
      bool ret = false;
      fs::File inFile = fs->open( filename );
      if( inFile ) {
        ret = load( (Stream*)&inFile );
        inFile.close();
      }
      fsduration = millis() - start;
      return ret;
    }

    bool save( Stream* stream )
    {
      uint32_t start = millis();
      bool ret = false;
      if( sprite->width()<=0 || sprite->height() <= 0 || sprite->bufferLength() <= 0 ) return false;
      SpriteFS_t spriteInfo = { (uint16_t)sprite->width(), (uint16_t)sprite->height(), sprite->bufferLength() };
      float bits_per_pixel = float(spriteInfo.buffer_length) / float( spriteInfo.width*spriteInfo.height ) * 8.0;
      if( bits_per_pixel <= 0 ) return false;
      size_t written_bytes = 0;
      written_bytes += stream->write( (char*)&spriteInfo, sizeof( SpriteFS_t ) );
      written_bytes += stream->write( (char*)sprite->getBuffer(), spriteInfo.buffer_length );
      log_d("Written %d*%d@%dbpp sprite (%d bytes)", spriteInfo.width, spriteInfo.height, int(bits_per_pixel), spriteInfo.buffer_length );
      streamduration = millis() - start;
      return written_bytes == spriteInfo.buffer_length + sizeof( SpriteFS_t );
    }

    bool load( Stream* stream )
    {
      uint32_t start = millis();
      bool ret = false;
      SpriteFS_t spriteInfo = {0,0,0};
      if( ! stream->readBytes( (uint8_t*)&spriteInfo, sizeof( SpriteFS_t ) ) ) return false;
      if( spriteInfo.width==0 || spriteInfo.height == 0 || spriteInfo.buffer_length == 0 ) return false;
      float bits_per_pixel = float(spriteInfo.buffer_length) / float( spriteInfo.width*spriteInfo.height ) * 8.0;
      if( bits_per_pixel <= 0 ) return false;
      log_d("Will create %d*%d@%dpp sprite (%d bytes)", spriteInfo.width, spriteInfo.height, int(bits_per_pixel), spriteInfo.buffer_length);
      if( !sprite->createSprite( spriteInfo.width, spriteInfo.height ) ) return false;
      sprite->setColorDepth( bits_per_pixel );
      ret = stream->readBytes( (uint8_t*)sprite->getBuffer(), spriteInfo.buffer_length );
      streamduration = millis() - start;
      return ret;
    }
};


LGFX_Sprite *sprite = new LGFX_Sprite( &M5.Lcd );


void setup()
{
  M5.begin();

  sprite->createSprite( 128, 128 );
  sprite->setColorDepth(1);
  sprite->fillSprite( TFT_WHITE );

  M5.Lcd.fillScreen( TFT_BLUE );

  SpriteFS spriteFS( sprite );

  if( spriteFS.save( &SD, "/white.sprite" ) ) {
    Serial.printf("Sprite saved! (fs: %dms, stream %dms)\n", spriteFS.fsduration, spriteFS.streamduration );
    if( spriteFS.load( &SD, "/white.sprite") ) {
      Serial.printf("Sprite loaded! (fs: %dms, stream %dms)\n", spriteFS.fsduration, spriteFS.streamduration );
      sprite->pushSprite(0,0);
    } else {
      Serial.println("Failed to reload sprite");
    }
  } else {
    Serial.println("Failed to save sprite");
  }
}


void loop()
{

}
