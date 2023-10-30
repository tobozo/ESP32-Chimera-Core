#pragma once

#include <vector>
#include "AVI_Types.hpp"

class AviMjpegEncoder
{
  public:

    AviMjpegEncoder( AVI_Params_t *params );

    // add a whole jpeg frame
    void addJpegFrame(const uint8_t *jpegData, int len);
    // add jpeg frame by chunks
    void writeJpegFrameHeader(int len=0);
    void writeJpegFrameData(const uint8_t *jpegData, int len);
    void writeJpegFrameFinish();

    void finalize();

    size_t framesCount() { return imgIndex.size(); }

    fs::AviFile aviFile;
    fs::AviFile idxFile;

  private:

    AVI_Params_t *params;

    // interesting offsets in the aviFile when finalizing
    uint32_t framesCountFieldPos1, framesCountFieldPos2, moviPos, jpegFramePos, jpegFrameSize;

    void writeAviHeader(); // Note: called from constructor
    void writeFrameHeader(int len);
    void writeFrame(const uint8_t *data, int len);
    void writeLengthField(const char* msg = nullptr);
    void finalizeLengthField(const char* msg = nullptr);
    std::vector<uint32_t> lengthFields;

    uint32_t idx_items_count = 0;
    void writeJpegIdxItem( fs::AviFile* dst, avi_idx_item_t item );
    std::vector<avi_idx_item_t>imgIndex;
    bool needs_finalize;

    const char* tmpIdxPath = "/tmp.idx";
    const char* descStr = "This video was created with ESP32-AVIEncoder - copyleft (c+) tobozo 2023";

};


