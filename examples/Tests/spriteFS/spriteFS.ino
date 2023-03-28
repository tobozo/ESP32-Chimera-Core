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

    bool save( fs::FS* fs, const char* filename )
    {
      bool ret = false;
      fs::File outFile = fs->open( filename, "w" );
      if( outFile ) {
        ret = save( (Stream*)&outFile );
        outFile.close();
      }
      return ret;
    }

    bool load( fs::FS* fs, const char* filename )
    {
      bool ret = false;
      fs::File inFile = fs->open( filename );
      if( inFile ) {
        ret = load( (Stream*)&inFile );
        inFile.close();
      }
      return ret;
    }

    bool save( Stream* stream )
    {
      bool ret = false;
      SpriteFS_t spriteInfo = { (uint16_t)sprite->width(), (uint16_t)sprite->height(), sprite->bufferLength() };
      if( spriteInfo.width==0 || spriteInfo.height == 0 || spriteInfo.buffer_length == 0 ) return false;
      size_t written_bytes = 0;
      written_bytes += stream->write( (char*)&spriteInfo, sizeof( SpriteFS_t ) );
      written_bytes += stream->write( (char*)sprite->getBuffer(), sprite->bufferLength() );
      log_d("Written %d*%d@%dbpp sprite (%d bytes)", spriteInfo.width, spriteInfo.height, sprite->getColorDepth(), spriteInfo.buffer_length );
      return written_bytes == sprite->bufferLength() + sizeof( SpriteFS_t );
    }

    bool load( Stream* stream )
    {
      bool ret = false;
      SpriteFS_t spriteInfo = {0,0,0};
      if( ! stream->readBytes( (uint8_t*)&spriteInfo, sizeof( SpriteFS_t ) ) ) return false;
      if( spriteInfo.width==0 || spriteInfo.height == 0 || spriteInfo.buffer_length == 0 ) return false;
      uint8_t bytes_per_pixel = spriteInfo.buffer_length / ( spriteInfo.width*spriteInfo.height );
      if( bytes_per_pixel == 0 ) return false;
      log_d("Will create %d*%d@%dbpp sprite (%d bytes)", spriteInfo.width, spriteInfo.height, bytes_per_pixel*8, spriteInfo.buffer_length);
      if( !sprite->createSprite( spriteInfo.width, spriteInfo.height ) ) return false;
      sprite->setColorDepth( 8*bytes_per_pixel );
      ret = stream->readBytes( (uint8_t*)sprite->getBuffer(), spriteInfo.buffer_length );
      return ret;
    }
};


LGFX_Sprite *sprite = new LGFX_Sprite( &M5.Lcd );


void setup()
{
  M5.begin();

  sprite->createSprite( 128, 128 );
  sprite->fillSprite( TFT_YELLOW );

  M5.Lcd.fillScreen( TFT_BLUE );

  SpriteFS spriteFS( sprite );

  if( spriteFS.save( &SD, "/yellow.sprite" ) ) {
    Serial.println("Sprite saved!");
    if( spriteFS.load( &SD, "/yellow.sprite") ) {
      Serial.println("Sprite loaded !");
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
