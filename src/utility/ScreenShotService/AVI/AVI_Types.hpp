#pragma once

#include <FS.h>
#include <FSImpl.h>

class AviMjpegEncoder;

#if defined __JPEGENC__ || __has_include(<JPEGENC.h>)
  #include <JPEGENC.h> // https://github.com/bitbank2/JPEGENC
  #define HAS_JPEGENC

  // for direct to disk (no jpeg buffer) avi encoding
  struct JPGENC_AVI_Proxy_t
  {
    static AviMjpegEncoder* aviEncoder;
    static void*   open(const char *filename);
    static int32_t write(JPEGFILE *p, uint8_t *buffer, int32_t length);
    static void    close(JPEGFILE *p);
    static int32_t read(JPEGFILE *p, uint8_t *buffer, int32_t length);
    static int32_t seek(JPEGFILE *p, int32_t position);
  };

#endif

// frame dimensions
struct avi_size_t
{
  uint32_t h, w;
};

// index items
struct avi_idx_item_t
{
  uint32_t pos, len;
};

// framebuffer to avi stream params model
struct AVI_Params_t
{
  AVI_Params_t( fs::FS* fs, const char* fpath=nullptr, size_t fps=24, bool use_buffer=true, bool use_index_file=true )
    : fs(fs), fps(fps), use_buffer(use_buffer), use_index_file(use_index_file)
  {
    if( fs ) ready=true;
    if( fpath ) path=String(fpath);
  }
  fs::FS* fs{nullptr};
  String path{""};
  avi_size_t size{0,0};
  size_t fps{24};
  bool use_buffer{true};
  bool use_index_file{true};
  bool ready{false};
};


// shamelessly overload espressif namespace
namespace fs
{
  // fs::File base implementation extended with writeDword(), writeWord() and writeWordStr()
  class AviFile : public fs::FileImpl
  {
    private:
      mutable fs::File _file;
    public:
      AviFile(fs::File file=FileImplPtr()) : _file(file) {}
      virtual ~AviFile() { }

      virtual size_t write(const uint8_t *buf, size_t size) { return _file.write(buf, size); }
      virtual size_t read(uint8_t* buf, size_t size) { return _file.read(buf, size); }
      virtual void flush() { return _file.flush(); }
      virtual size_t position() const { return _file.position(); }
      virtual size_t size() const { return _file.size(); }
      virtual void close() { _file.close(); }
      virtual operator bool() { return _file.operator bool(); }
      virtual bool isDirectory(void) { return _file.isDirectory(); }
      virtual fs::FileImplPtr openNextFile(const char* mode) { return  std::make_shared<AviFile>(_file.openNextFile(mode)); }
      virtual bool seekDir(long position) { return _file.seek(position); }
      virtual bool seek(uint32_t pos, fs::SeekMode mode=SeekSet) { return _file.seek( pos, mode ); }
      virtual const char* name() const { return _file.name(); }
      virtual const char* path() const {
        #if defined ESP_IDF_VERSION_MAJOR && ESP_IDF_VERSION_MAJOR >= 4
          return _file.path();
        #else
          return _file.name();
        #endif
      }
      virtual String getNextFileName(void) { /* not implemented and not needed */ return String("Unimplemented"); }
      virtual String getNextFileName(bool*) { /* not implemented and not needed */ return String("Unimplemented"); }
      virtual time_t getLastWrite() { /* not implemented and not needed */  return 0; }
      virtual bool setBufferSize(size_t size) { /* not implemented and not needed */ return false; }
      virtual void rewindDirectory(void) { /* not implemented and not needed */  }

      virtual size_t readBytes( char* buf, size_t length ) { return _file.readBytes( buf, length ); }
      virtual size_t write(const uint8_t byte) { return _file.write(byte); }
      virtual uint8_t read() { return _file.read(); }
      virtual bool available() { return _file.available(); }

      virtual size_t writeDword(uint32_t word) { return _file.write((const uint8_t *)&word, 4); }
      virtual size_t writeWord(uint16_t word) { return _file.write((const uint8_t *)&word, 2); }
      virtual size_t writeWordStr(const char* word) { return _file.write((const uint8_t *)word, 4); }
  };
};
