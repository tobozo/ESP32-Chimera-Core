/*
 *
 * TinyJPEGEncoder
 *
 * Copyright 2019 tobozo http://github.com/tobozo
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files ("TinyBMPEncoder"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This is a readable and simple single-header JPEG encoder.
 *
 * Features
 *  - Implements Baseline DCT JPEG compression.
 *  - No dynamic allocations.
 *  - Writes to SD/SD_MMC/SPIFFS
 *
 * ==== Thanks ====
 *
 *  AssociationSirius (Bug reports)
 *  Bernard van Gastel (Thread-safe defaults, BSD compilation)
 *  Sergio Gonzalez (initial C/std library https://github.com/serge-rgb/TinyJPEG)
 *
 * ==== License ====
 *
 * This software is in the public domain. Where that dedication is not
 * recognized, you are granted a perpetual, irrevocable license to copy and
 * modify this file as you see fit.
 *
 * === CHANGES by tobozo Nov. 2019===
 *
 *  - uses psram if available
 *  - uses fs::FS
 *  - rewrotecode to in a C++ style
 *
 */


#pragma once
#define __TINY_JPEG_ENCODER__

#include "../../../M5Display.h"
// C std lib
#include <assert.h>
#include <inttypes.h>
#include <math.h>   // floorf, ceilf
#include <stdio.h>  // FILE, puts
#include <string.h> // memcpy

// Only use zero for debugging and/or inspection.
#define TJE_USE_FAST_DCT 1

#define TJEI_BUFFER_SIZE 512
#ifndef NDEBUG
  #define tje_log(msg) log_e(msg)
#else  // NDEBUG
  #define tje_log(msg)
#endif  // NDEBUG

// ============================================================
// The following structs exist only for code clarity, debugability, and
// readability. They are used when writing to disk, but it is useful to have
// 1-packed-structs to document how the format works, and to inspect memory
// while developing.
// ============================================================

static const uint8_t tjeik_jfif_id[] = "JFIF";
static const uint8_t tjeik_com_str[] = "Created by Tiny JPEG Encoder (https://github.com/serge-rgb/TinyJPEG)";

typedef void writeFunc( fs::File jpegFile, void* data, int size );

typedef struct {
  fs::File   jpegFile;
  writeFunc* func;
} TJEWriteContext;

typedef struct {
  // Huffman data.
  uint8_t         ehuffsize[4][257];
  uint16_t        ehuffcode[4][256];
  uint8_t const * ht_bits[4];
  uint8_t const * ht_vals[4];
  // Cuantization tables.
  uint8_t         qt_luma[64];
  uint8_t         qt_chroma[64];
  // fwrite by default. User-defined when using encodeWithFunc.
  TJEWriteContext writeContext;
  // Buffered output. Big performance win when using the usual stdlib implementations.
  size_t          outputBufferCount;
  uint8_t         outputBuffer[TJEI_BUFFER_SIZE];
} TJEState;

// TODO: Get rid of packed structs!
#pragma pack(push)
#pragma pack(1)
typedef struct {
  uint16_t SOI;
  // JFIF header.
  uint16_t APP0;
  uint16_t jfif_len;
  uint8_t  jfif_id[5];
  uint16_t version;
  uint8_t  units;
  uint16_t x_density;
  uint16_t y_density;
  uint8_t  x_thumb;
  uint8_t  y_thumb;
} TJEJPEGHeader;

typedef struct {
  uint16_t com;
  uint16_t com_len;
  char     com_str[sizeof(tjeik_com_str) - 1];
} TJEJPEGComment;

// Helper struct for TJEFrameHeader (below).
typedef struct {
  uint8_t  component_id;
  uint8_t  sampling_factors;    // most significant 4 bits: horizontal. 4 LSB: vertical (A.1.1)
  uint8_t  qt;                  // Quantization table selector.
} TJEComponentSpec;

typedef struct {
  uint16_t         SOF;
  uint16_t         len;                   // 8 + 3 * frame.numComponents
  uint8_t          precision;             // Sample precision (bits per sample).
  uint16_t         height;
  uint16_t         width;
  uint8_t          numComponents;        // For this implementation, will be equal to 3.
  TJEComponentSpec component_spec[3];
} TJEFrameHeader;

typedef struct {
  uint8_t component_id;                 // Just as with TJEComponentSpec
  uint8_t dc_ac;                        // (dc|ac)
} TJEFrameComponentSpec;

typedef struct {
  uint16_t              SOS;
  uint16_t              len;
  uint8_t               numComponents;  // 3.
  TJEFrameComponentSpec component_spec[3];
  uint8_t               first;  // 0
  uint8_t               last;  // 63
  uint8_t               ah_al;  // o
} TJEScanHeader;

enum {
  TJEI_LUMA_DC,
  TJEI_LUMA_AC,
  TJEI_CHROMA_DC,
  TJEI_CHROMA_AC,
};

#if TJE_USE_FAST_DCT
struct TJEProcessedQT {
  float chroma[64];
  float luma[64];
};
#endif

typedef enum {
  TJEI_DC = 0,
  TJEI_AC = 1
} TJEHuffmanTableClass;


#pragma pack(pop)

typedef void (*jpeg_encoder_callback_t)(uint32_t y, uint32_t h, unsigned char* rgb_buffer, void* device);

class JPEG_Encoder {
  public:

    JPEG_Encoder( fs::FS * fileSystem ) : _fileSystem( fileSystem ) { }

    fs::File jpegFile;
    fs::FS * _fileSystem;

    uint8_t**  huffsize = NULL;
    uint16_t** huffcode = NULL;

    void       begin( bool ifPsram = true );
    int        encodeToFile( const char* dest_path, const int width, const int height, const int numComponents, unsigned char* src_data, jpeg_encoder_callback_t fp, void* device );
    int        encodeWithFunc( writeFunc* func, fs::File jpegFile, const int quality, const int width, const int height, const int numComponents, unsigned char* src_data, jpeg_encoder_callback_t fp, void* device );

  private:

    int        encodeMain( TJEState* state, unsigned char* src_data, jpeg_encoder_callback_t fp, void* device, const int width, const int height, const int srcNumComponents );
    int        encodeToFileAtQuality( const char* dest_path, const int quality, const int width, const int height, const int numComponents, unsigned char* src_data, jpeg_encoder_callback_t fp, void* device );
    void       encodeAndWriteMCU( TJEState*, float*, float*, uint8_t*, uint16_t*, uint8_t*, uint16_t*, int*, uint32_t*, uint32_t* );
    void       write( TJEState* state, const void* data, size_t num_bytes, size_t num_elements );
    void       write_DQT( TJEState* state, const uint8_t* matrix, uint8_t id );
    void       write_DHT( TJEState* state, uint8_t const * matrix_len, uint8_t const * matrix_val, TJEHuffmanTableClass ht_class, uint8_t id );
    void       writeBits( TJEState* state, uint32_t* bitbuffer, uint32_t* location, uint16_t num_bits, uint16_t bits );

    uint8_t*   huffGetCodeLengths( uint8_t huffsize[/*256*/], uint8_t const * bits );
    uint16_t*  huffGetCodes( uint16_t codes[], uint8_t* huffsize, int64_t count );
    void       huffGetExtended( uint8_t* out_ehuffsize, uint16_t* out_ehuffcode, uint8_t const * huffval, uint8_t* huffsize, uint16_t* huffcode, int64_t count );
    void       huffExpand( TJEState* state );
    void       calcVarLengthAsInt( int value, uint16_t out[2] );
    void       fast_fdct( float * data );


}; // end class

