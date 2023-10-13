// https://github.com/charlietangora/gif-h
// gif.h
// by Charlie Tangora
// Public domain.
// Email me : ctangora -at- gmail -dot- com
//
// This file offers a simple, very limited way to create animated GIFs directly in code.
//
// Those looking for particular cleverness are likely to be disappointed; it's pretty
// much a straight-ahead implementation of the GIF format with optional Floyd-Steinberg
// dithering. (It does at least use delta encoding - only the changed portions of each
// frame are saved.)
//
// So resulting files are often quite large. The hope is that it will be handy nonetheless
// as a quick and easily-integrated way for programs to spit out animations.
//
// Only RGBA8 is currently supported as an input format. (The alpha is ignored.)
//
// If capturing a buffer with a bottom-left origin (such as OpenGL), define GIF_FLIP_VERT
// to automatically flip the buffer data when writing the image (the buffer itself is
// unchanged.
//
// USAGE:
// Create a Gifwriter struct. Pass it to GifBegin() to initialize and write the header.
// Pass subsequent frames to GifwriteFrame().
// Finally, call GifEnd() to close the file handle and free memory.
//

#include "TinyGIFEncoder.hpp"


void GIF_Encoder::init() {
  if( !psramInit() ) {
    log_n("[INFO] No PSRAM found, GIF Encoding won't work");
  }
}


bool GIF_Encoder::encodeToFile( const char* filename, const int imageW, const int imageH ) {
  return encodeToFile( filename, 0, 0, imageW, imageH );
}


bool GIF_Encoder::encodeToFile( const char* filename, const int imageX, const int imageY, const int width, const int height )
{
  uint8_t *imageRGB = (uint8_t*)ps_calloc( width*height*3, sizeof(uint8_t) );
  if( imageRGB == nullptr ) {
    log_n("Unable to allocate %d bytes for rgb image capture", width*height*3 );
  }
  uint8_t *imageRGBA = (uint8_t*)ps_calloc( width*height*4, sizeof(uint8_t) );
  if( imageRGBA == nullptr ) {
    log_n("Unable to allocate %d bytes for rgba image capture", width*height*4 );
  }

  _tft->readRectRGB(imageX, imageY, width, height, imageRGB );

  int n = 4 * width * height;
  int s = 0, d = 0;
  while (d < n) {
    imageRGBA[d++] = 0+imageRGB[s++];
    imageRGBA[d++] = 0+imageRGB[s++];
    imageRGBA[d++] = 0+imageRGB[s++];
    imageRGBA[d++] = 255;    // skip the alpha byte
  }

  int frameDelay = 0;
  bool success = false;

  if( begin(*_fileSystem, filename, width, height, frameDelay) ) {
    success = writeFrame((uint8_t*)imageRGBA, width, height, frameDelay);
  } else {
    log_n("Failed to created GIF :-(");
  }

  free( imageRGB );
  free( imageRGBA );

  end();

  return success;
}


// max, min, and abs functions
int GIF_Encoder::IMax(int l, int r)
{
  return l>r?l:r;
}
int GIF_Encoder::IMin(int l, int r)
{
  return l<r?l:r;
}
int GIF_Encoder::IAbs(int i)
{
  return i<0?-i:i;
}

// walks the k-d tree to pick the palette entry for a desired color.
// Takes as in/out parameters the current best color and its error -
// only changes them if it finds a better color in its subtree.
// this is the major hotspot in the code at the moment.
void GIF_Encoder::GetClosestPaletteColor(GifPalette* pPal, int r, int g, int b, int& bestInd, int& bestDiff, int treeRoot)
{
  // base case, reached the bottom of the tree
  if(treeRoot > (1<<pPal->bitDepth)-1) {
    int ind = treeRoot-(1<<pPal->bitDepth);
    if(ind == kGifTransIndex) return;

    // check whether this color is better than the current winner
    int r_err = r - ((int32_t)pPal->r[ind]);
    int g_err = g - ((int32_t)pPal->g[ind]);
    int b_err = b - ((int32_t)pPal->b[ind]);
    int diff = IAbs(r_err)+IAbs(g_err)+IAbs(b_err);

    if(diff < bestDiff) {
      bestInd = ind;
      bestDiff = diff;
    }
    return;
  }

  // take the appropriate color (r, g, or b) for this node of the k-d tree
  int comps[3]; comps[0] = r; comps[1] = g; comps[2] = b;
  int splitComp = comps[pPal->treeSplitElt[treeRoot]];

  int splitPos = pPal->treeSplit[treeRoot];
  if(splitPos > splitComp) {
    // check the left subtree
    GetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot*2);
    if( bestDiff > splitPos - splitComp ) {
      // cannot prove there's not a better value in the right subtree, check that too
      GetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot*2+1);
    }
  } else {
    GetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot*2+1);
    if( bestDiff > splitComp - splitPos ) {
      GetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot*2);
    }
  }
}


void GIF_Encoder::SwapPixels(uint8_t* image, int pixA, int pixB)
{
  uint8_t rA = image[pixA*4];
  uint8_t gA = image[pixA*4+1];
  uint8_t bA = image[pixA*4+2];
  uint8_t aA = image[pixA*4+3];

  uint8_t rB = image[pixB*4];
  uint8_t gB = image[pixB*4+1];
  uint8_t bB = image[pixB*4+2];
  uint8_t aB = image[pixA*4+3];

  image[pixA*4] = rB;
  image[pixA*4+1] = gB;
  image[pixA*4+2] = bB;
  image[pixA*4+3] = aB;

  image[pixB*4] = rA;
  image[pixB*4+1] = gA;
  image[pixB*4+2] = bA;
  image[pixB*4+3] = aA;
}

// just the partition operation from quicksort
int GIF_Encoder::Partition(uint8_t* image, const int left, const int right, const int elt, int pivotIndex)
{
  const int pivotValue = image[(pivotIndex)*4+elt];
  SwapPixels(image, pivotIndex, right-1);
  int storeIndex = left;
  bool split = 0;
  for(int ii=left; ii<right-1; ++ii) {
    int arrayVal = image[ii*4+elt];
    if( arrayVal < pivotValue ) {
      SwapPixels(image, ii, storeIndex);
      ++storeIndex;
    } else if( arrayVal == pivotValue ) {
      if(split) {
        SwapPixels(image, ii, storeIndex);
        ++storeIndex;
      }
      split = !split;
    }
  }
  SwapPixels(image, storeIndex, right-1);
  return storeIndex;
}

// Perform an incomplete sort, finding all elements above and below the desired median
void GIF_Encoder::PartitionByMedian(uint8_t* image, int left, int right, int com, int neededCenter)
{
  if(left < right-1) {
    int pivotIndex = left + (right-left)/2;
    pivotIndex = Partition(image, left, right, com, pivotIndex);
    // Only "sort" the section of the array that contains the median
    if(pivotIndex > neededCenter)
      PartitionByMedian(image, left, pivotIndex, com, neededCenter);
    if(pivotIndex < neededCenter)
      PartitionByMedian(image, pivotIndex+1, right, com, neededCenter);
  }
}

// Builds a palette by creating a balanced k-d tree of all pixels in the image
void GIF_Encoder::SplitPalette(uint8_t* image, int numPixels, int firstElt, int lastElt, int splitElt, int splitDist, int treeNode, bool buildForDither, GifPalette* pal)
{
  if(lastElt <= firstElt || numPixels == 0)
      return;

  // base case, bottom of the tree
  if(lastElt == firstElt+1) {
    if(buildForDither) {
      // Dithering needs at least one color as dark as anything
      // in the image and at least one brightest color -
      // otherwise it builds up error and produces strange artifacts
      if( firstElt == 1 ) {
        // special case: the darkest color in the image
        uint32_t r=255, g=255, b=255;
        for(int ii=0; ii<numPixels; ++ii) {
          r = (uint32_t)IMin((int32_t)r, image[ii * 4 + 0]);
          g = (uint32_t)IMin((int32_t)g, image[ii * 4 + 1]);
          b = (uint32_t)IMin((int32_t)b, image[ii * 4 + 2]);
        }
        pal->r[firstElt] = (uint8_t)r;
        pal->g[firstElt] = (uint8_t)g;
        pal->b[firstElt] = (uint8_t)b;
        return;
      }

      if( firstElt == (1 << pal->bitDepth)-1 ) {
        // special case: the lightest color in the image
        uint32_t r=0, g=0, b=0;
        for(int ii=0; ii<numPixels; ++ii) {
          r = (uint32_t)IMax((int32_t)r, image[ii * 4 + 0]);
          g = (uint32_t)IMax((int32_t)g, image[ii * 4 + 1]);
          b = (uint32_t)IMax((int32_t)b, image[ii * 4 + 2]);
        }
        pal->r[firstElt] = (uint8_t)r;
        pal->g[firstElt] = (uint8_t)g;
        pal->b[firstElt] = (uint8_t)b;
        return;
      }
    }

    // otherwise, take the average of all colors in this subcube
    uint64_t r=0, g=0, b=0;
    for(int ii=0; ii<numPixels; ++ii) {
      r += image[ii*4+0];
      g += image[ii*4+1];
      b += image[ii*4+2];
    }

    r += (uint64_t)numPixels / 2;  // round to nearest
    g += (uint64_t)numPixels / 2;
    b += (uint64_t)numPixels / 2;

    r /= (uint64_t)numPixels;
    g /= (uint64_t)numPixels;
    b /= (uint64_t)numPixels;

    pal->r[firstElt] = (uint8_t)r;
    pal->g[firstElt] = (uint8_t)g;
    pal->b[firstElt] = (uint8_t)b;

    return;
  }

  // Find the axis with the largest range
  int minR = 255, maxR = 0;
  int minG = 255, maxG = 0;
  int minB = 255, maxB = 0;
  for(int ii=0; ii<numPixels; ++ii) {
    int r = image[ii*4+0];
    int g = image[ii*4+1];
    int b = image[ii*4+2];

    if(r > maxR) maxR = r;
    if(r < minR) minR = r;

    if(g > maxG) maxG = g;
    if(g < minG) minG = g;

    if(b > maxB) maxB = b;
    if(b < minB) minB = b;
  }

  int rRange = maxR - minR;
  int gRange = maxG - minG;
  int bRange = maxB - minB;

  // and split along that axis. (incidentally, this means this isn't a "proper" k-d tree but I don't know what else to call it)
  int splitCom = 1;
  if(bRange > gRange) splitCom = 2;
  if(rRange > bRange && rRange > gRange) splitCom = 0;

  int subPixelsA = numPixels * (splitElt - firstElt) / (lastElt - firstElt);
  int subPixelsB = numPixels-subPixelsA;

  PartitionByMedian(image, 0, numPixels, splitCom, subPixelsA);

  pal->treeSplitElt[treeNode] = (uint8_t)splitCom;
  pal->treeSplit[treeNode] = image[subPixelsA*4+splitCom];

  SplitPalette(image,              subPixelsA, firstElt, splitElt, splitElt-splitDist, splitDist/2, treeNode*2,   buildForDither, pal);
  SplitPalette(image+subPixelsA*4, subPixelsB, splitElt, lastElt,  splitElt+splitDist, splitDist/2, treeNode*2+1, buildForDither, pal);
}

// Finds all pixels that have changed from the previous image and
// moves them to the fromt of th buffer.
// This allows us to build a palette optimized for the colors of the
// changed pixels only.
int GIF_Encoder::PickChangedPixels( const uint8_t* lastFrame, uint8_t* frame, int numPixels )
{
  int numChanged = 0;
  uint8_t* writeIter = frame;

  for (int ii=0; ii<numPixels; ++ii) {
    if(lastFrame[0] != frame[0] ||
      lastFrame[1] != frame[1] ||
      lastFrame[2] != frame[2])
    {
      writeIter[0] = frame[0];
      writeIter[1] = frame[1];
      writeIter[2] = frame[2];
      ++numChanged;
      writeIter += 4;
    }
    lastFrame += 4;
    frame += 4;
  }

  return numChanged;
}

// Creates a palette by placing all the image pixels in a k-d tree and then averaging the blocks at the bottom.
// This is known as the "modified median split" technique
void GIF_Encoder::MakePalette( const uint8_t* lastFrame, const uint8_t* nextFrame, uint32_t width, uint32_t height, int bitDepth, bool buildForDither, GifPalette* pPal )
{
  pPal->bitDepth = bitDepth;

  // SplitPalette is destructive (it sorts the pixels by color) so
  // we must create a copy of the image for it to destroy
  size_t imageSize = (size_t)(width * height * 4 * sizeof(uint8_t));
  uint8_t* destroyableImage = (uint8_t*)GIF_TEMP_MALLOC(imageSize);
  if( destroyableImage == nullptr ) {
    log_n("Unable to allocate %d bytes", imageSize);
  }
  memcpy(destroyableImage, nextFrame, imageSize);

  int numPixels = (int)(width * height);
  if(lastFrame)
    numPixels = PickChangedPixels(lastFrame, destroyableImage, numPixels);

  const int lastElt = 1 << bitDepth;
  const int splitElt = lastElt/2;
  const int splitDist = splitElt/2;

  SplitPalette(destroyableImage, numPixels, 1, lastElt, splitElt, splitDist, 1, buildForDither, pPal);

  GIF_TEMP_FREE(destroyableImage);

  // add the bottom node for the transparency index
  pPal->treeSplit[1 << (bitDepth-1)] = 0;
  pPal->treeSplitElt[1 << (bitDepth-1)] = 0;

  pPal->r[0] = pPal->g[0] = pPal->b[0] = 0;
}

// Implements Floyd-Steinberg dithering, writes palette value to alpha
void GIF_Encoder::DitherImage( const uint8_t* lastFrame, const uint8_t* nextFrame, uint8_t* outFrame, uint32_t width, uint32_t height, GifPalette* pPal )
{
  int numPixels = (int)(width * height);

  // quantPixels initially holds color*256 for all pixels
  // The extra 8 bits of precision allow for sub-single-color error values
  // to be propagated
  int32_t *quantPixels = (int32_t *)GIF_TEMP_MALLOC(sizeof(int32_t) * (size_t)numPixels * 4);
  if( quantPixels == nullptr ) {
    log_n("Unable to allocate %d bytes", sizeof(int32_t) * (size_t)numPixels * 4);
  }

  for( int ii=0; ii<numPixels*4; ++ii )
  {
    uint8_t pix = nextFrame[ii];
    int32_t pix16 = int32_t(pix) * 256;
    quantPixels[ii] = pix16;
  }

  for( uint32_t yy=0; yy<height; ++yy ) {
    for( uint32_t xx=0; xx<width; ++xx ) {
      int32_t* nextPix = quantPixels + 4*(yy*width+xx);
      const uint8_t* lastPix = lastFrame? lastFrame + 4*(yy*width+xx) : NULL;

      // Compute the colors we want (rounding to nearest)
      int32_t rr = (nextPix[0] + 127) / 256;
      int32_t gg = (nextPix[1] + 127) / 256;
      int32_t bb = (nextPix[2] + 127) / 256;

      // if it happens that we want the color from last frame, then just write out
      // a transparent pixel
      if( lastFrame &&
        lastPix[0] == rr &&
        lastPix[1] == gg &&
        lastPix[2] == bb )
      {
        nextPix[0] = rr;
        nextPix[1] = gg;
        nextPix[2] = bb;
        nextPix[3] = kGifTransIndex;
        continue;
      }

      int bestDiff = 1000000;
      int bestInd = kGifTransIndex;

      // Search the palete
      GetClosestPaletteColor(pPal, rr, gg, bb, bestInd, bestDiff);

      // write the result to the temp buffer
      int32_t r_err = nextPix[0] - int32_t(pPal->r[bestInd]) * 256;
      int32_t g_err = nextPix[1] - int32_t(pPal->g[bestInd]) * 256;
      int32_t b_err = nextPix[2] - int32_t(pPal->b[bestInd]) * 256;

      nextPix[0] = pPal->r[bestInd];
      nextPix[1] = pPal->g[bestInd];
      nextPix[2] = pPal->b[bestInd];
      nextPix[3] = bestInd;

      // Propagate the error to the four adjacent locations
      // that we haven't touched yet
      int quantloc_7 = (int)(yy * width + xx + 1);
      int quantloc_3 = (int)(yy * width + width + xx - 1);
      int quantloc_5 = (int)(yy * width + width + xx);
      int quantloc_1 = (int)(yy * width + width + xx + 1);

      if(quantloc_7 < numPixels){
        int32_t* pix7 = quantPixels+4*quantloc_7;
        pix7[0] += IMax( -pix7[0], r_err * 7 / 16 );
        pix7[1] += IMax( -pix7[1], g_err * 7 / 16 );
        pix7[2] += IMax( -pix7[2], b_err * 7 / 16 );
      }

      if(quantloc_3 < numPixels){
        int32_t* pix3 = quantPixels+4*quantloc_3;
        pix3[0] += IMax( -pix3[0], r_err * 3 / 16 );
        pix3[1] += IMax( -pix3[1], g_err * 3 / 16 );
        pix3[2] += IMax( -pix3[2], b_err * 3 / 16 );
      }

      if(quantloc_5 < numPixels){
        int32_t* pix5 = quantPixels+4*quantloc_5;
        pix5[0] += IMax( -pix5[0], r_err * 5 / 16 );
        pix5[1] += IMax( -pix5[1], g_err * 5 / 16 );
        pix5[2] += IMax( -pix5[2], b_err * 5 / 16 );
      }

      if(quantloc_1 < numPixels){
        int32_t* pix1 = quantPixels+4*quantloc_1;
        pix1[0] += IMax( -pix1[0], r_err / 16 );
        pix1[1] += IMax( -pix1[1], g_err / 16 );
        pix1[2] += IMax( -pix1[2], b_err / 16 );
      }
    }
  }

  // Copy the palettized result to the output buffer
  for( int ii=0; ii<numPixels*4; ++ii ) {
    outFrame[ii] = (uint8_t)quantPixels[ii];
  }

  GIF_TEMP_FREE(quantPixels);
}

// Picks palette colors for the image using simple thresholding, no dithering
void GIF_Encoder::ThresholdImage( const uint8_t* lastFrame, const uint8_t* nextFrame, uint8_t* outFrame, uint32_t width, uint32_t height, GifPalette* pPal )
{
  uint32_t numPixels = width*height;
  for( uint32_t ii=0; ii<numPixels; ++ii ) {
    // if a previous color is available, and it matches the current color,
    // set the pixel to transparent
    if(lastFrame &&
      lastFrame[0] == nextFrame[0] &&
      lastFrame[1] == nextFrame[1] &&
      lastFrame[2] == nextFrame[2])
    {
      outFrame[0] = lastFrame[0];
      outFrame[1] = lastFrame[1];
      outFrame[2] = lastFrame[2];
      outFrame[3] = kGifTransIndex;
    } else {
      // palettize the pixel
      int bestDiff = 1000000;
      int bestInd = 1;
      GetClosestPaletteColor(pPal, nextFrame[0], nextFrame[1], nextFrame[2], bestInd, bestDiff);

      // write the resulting color to the output buffer
      outFrame[0] = pPal->r[bestInd];
      outFrame[1] = pPal->g[bestInd];
      outFrame[2] = pPal->b[bestInd];
      outFrame[3] = (uint8_t)bestInd;
    }

    if(lastFrame) lastFrame += 4;
    outFrame += 4;
    nextFrame += 4;
  }
}

// insert a single bit
void GIF_Encoder::writeBit( GifBitStatus& stat, uint32_t bit )
{
  bit = bit & 1;
  bit = bit << stat.bitIndex;
  stat.byte |= bit;

  ++stat.bitIndex;
  if( stat.bitIndex > 7 ) {
    // move the newly-finished byte to the chunk buffer
    stat.chunk[stat.chunkIndex++] = stat.byte;
    // and start a new byte
    stat.bitIndex = 0;
    stat.byte = 0;
  }
}

// write all bytes so far to the file
void GIF_Encoder::writeChunk( fs::File f, GifBitStatus& stat )
{
  f.write((int)stat.chunkIndex);
  f.write(stat.chunk, stat.chunkIndex);
  stat.bitIndex = 0;
  stat.byte = 0;
  stat.chunkIndex = 0;
}

void GIF_Encoder::writeCode( fs::File f, GifBitStatus& stat, uint32_t code, uint32_t length )
{
  for( uint32_t ii=0; ii<length; ++ii ) {
    writeBit(stat, code);
    code = code >> 1;

    if( stat.chunkIndex == 255 ) {
      writeChunk(f, stat);
    }
  }
}



// write a 256-color (8-bit) image palette to the file
void GIF_Encoder::writePalette( const GifPalette* pPal, fs::File f )
{
  f.write(0);  // first color: transparency
  f.write(0);
  f.write(0);

  for(int ii=1; ii<(1 << pPal->bitDepth); ++ii) {
    uint32_t r = pPal->r[ii];
    uint32_t g = pPal->g[ii];
    uint32_t b = pPal->b[ii];

    f.write((int)r);
    f.write((int)g);
    f.write((int)b);
  }
}

// write the image header, LZW-compress and write out the image
void GIF_Encoder::writeLzwImage(fs::File f, uint8_t* image, uint32_t left, uint32_t top,  uint32_t width, uint32_t height, uint32_t frameDelay, GifPalette* pPal)
{
  // graphics control extension
  f.write(0x21);
  f.write(0xf9);
  f.write(0x04);
  f.write(0x05); // leave prev frame in place, this frame has transparency
  f.write(frameDelay & 0xff);
  f.write((frameDelay >> 8) & 0xff);
  f.write(kGifTransIndex); // transparent color index
  f.write(0);

  f.write(0x2c); // image descriptor block

  f.write(left & 0xff);           // corner of image in canvas space
  f.write((left >> 8) & 0xff);
  f.write(top & 0xff);
  f.write((top >> 8) & 0xff);

  f.write(width & 0xff);          // width and height of image
  f.write((width >> 8) & 0xff);
  f.write(height & 0xff);
  f.write((height >> 8) & 0xff);

  //f.write(0, f); // no local color table, no transparency
  //f.write(0x80, f); // no local color table, but transparency

  f.write(0x80 + pPal->bitDepth-1); // local color table present, 2 ^ bitDepth entries
  writePalette(pPal, f);

  const int minCodeSize = pPal->bitDepth;
  const uint32_t clearCode = 1 << pPal->bitDepth;

  f.write(minCodeSize); // min code size 8 bits

  GifLzwNode* codetree = (GifLzwNode*)GIF_TEMP_MALLOC(sizeof(GifLzwNode)*4096);
  if( codetree == nullptr ) {
    log_n("Unable to allocate %d bytes", sizeof(GifLzwNode)*4096);
  }

  memset(codetree, 0, sizeof(GifLzwNode)*4096);
  int32_t curCode = -1;
  uint32_t codeSize = (uint32_t)minCodeSize + 1;
  uint32_t maxCode = clearCode+1;

  GifBitStatus stat;
  stat.byte = 0;
  stat.bitIndex = 0;
  stat.chunkIndex = 0;

  writeCode(f, stat, clearCode, codeSize);  // start with a fresh LZW dictionary

  for(uint32_t yy=0; yy<height; ++yy) {
    for(uint32_t xx=0; xx<width; ++xx) {
      #ifdef GIF_FLIP_VERT
      // bottom-left origin image (such as an OpenGL capture)
        uint8_t nextValue = image[((height-1-yy)*width+xx)*4+3];
      #else
      // top-left origin
        uint8_t nextValue = image[(yy*width+xx)*4+3];
      #endif
      // "loser mode" - no compression, every single code is followed immediately by a clear
      //writeCode( f, stat, nextValue, codeSize );
      //writeCode( f, stat, 256, codeSize );
      if( curCode < 0 ) {
        // first value in a new run
        curCode = nextValue;
      } else if( codetree[curCode].m_next[nextValue] ) {
        // current run already in the dictionary
        curCode = codetree[curCode].m_next[nextValue];
      } else {
        // finish the current run, write a code
        writeCode(f, stat, (uint32_t)curCode, codeSize);
        // insert the new run into the dictionary
        codetree[curCode].m_next[nextValue] = (uint16_t)++maxCode;
        if( maxCode >= (1ul << codeSize) ) {
          // dictionary entry count has broken a size barrier,
          // we need more bits for codes
          codeSize++;
        }
        if( maxCode == 4095 ) {
          // the dictionary is full, clear it out and begin anew
          writeCode(f, stat, clearCode, codeSize); // clear tree

          memset(codetree, 0, sizeof(GifLzwNode)*4096);
          codeSize = (uint32_t)(minCodeSize + 1);
          maxCode = clearCode+1;
        }

        curCode = nextValue;
      }
    }
  }

  // compression footer
  writeCode(f, stat, (uint32_t)curCode, codeSize);
  writeCode(f, stat, clearCode, codeSize);
  writeCode(f, stat, clearCode + 1, (uint32_t)minCodeSize + 1);

  // write out the last partial chunk
  while( stat.bitIndex ) writeBit(stat, 0);
  if( stat.chunkIndex ) writeChunk(f, stat);

  f.write(0); // image block terminator

  GIF_TEMP_FREE(codetree);
}



// Creates a gif file.
// The input GIFwriter is assumed to be uninitialized.
// The delay value is the time between frames in hundredths of a second - note that not all viewers pay much attention to this value.
bool GIF_Encoder::begin( fs::FS &fs, const char* filename, uint32_t width, uint32_t height, uint32_t frameDelay, int32_t bitDepth, bool dither )
{
  (void)bitDepth; (void)dither; // Mute "Unused argument" warnings

  log_d("Will open %s file for writing", filename);
  outFile = fs.open( filename, "wb" );

  if(!outFile) {
    log_n("Could not open %s file for writing", filename);
    return false;
  }

  writerFirstFrame = true;

  log_d("Will try to allocate %d bytes", width*height*4);
  // allocate
  writerOldImage = (uint8_t*)GIF_MALLOC(width*height*4);
  if( writerOldImage == nullptr ) {
    log_n("Unable to allocate %d bytes", width*height*4);
    return false;
  }

  outFile.print("GIF89a");

  // screen descriptor
  outFile.write(width & 0xff);
  outFile.write((width >> 8) & 0xff);
  outFile.write(height & 0xff);
  outFile.write((height >> 8) & 0xff);

  outFile.write(0xf0);  // there is an unsorted global color table of 2 entries
  outFile.write(0);     // background color
  outFile.write(0);     // pixels are square (we need to specify this because it's 1989)

  // now the "global" palette (really just a dummy palette)
  // color 0: black
  outFile.write(0);
  outFile.write(0);
  outFile.write(0);
  // color 1: also black
  outFile.write(0);
  outFile.write(0);
  outFile.write(0);

  if( frameDelay != 0 ) {
    // animation header
    outFile.write(0x21); // extension
    outFile.write(0xff); // application specific
    outFile.write(11); // length 11
    outFile.print("NETSCAPE2.0"); // yes, really
    outFile.write(3); // 3 bytes of NETSCAPE2.0 data

    outFile.write(1); // JUST BECAUSE
    outFile.write(0); // loop infinitely (byte 0)
    outFile.write(0); // loop infinitely (byte 1)

    outFile.write(0); // block terminator
  }

  return true;
}

// writes out a new frame to a GIF in progress.
// The GIFwriter should have been created by GIFBegin.
// AFAIK, it is legal to use different bit depths for different frames of an image -
// this may be handy to save bits in animations that don't change much.
bool GIF_Encoder::writeFrame( const uint8_t* image, uint32_t width, uint32_t height, uint32_t frameDelay, int bitDepth, bool dither )
{
  if(!outFile) return false;

  const uint8_t* oldImage = writerFirstFrame? NULL : writerOldImage;
  writerFirstFrame = false;

  GifPalette pal;
  MakePalette((dither? NULL : oldImage), image, width, height, bitDepth, dither, &pal);

  if(dither)
    DitherImage(oldImage, image, writerOldImage, width, height, &pal);
  else
    ThresholdImage(oldImage, image, writerOldImage, width, height, &pal);

  writeLzwImage(outFile, writerOldImage, 0, 0, width, height, frameDelay, &pal);

  return true;
}

// writes the EOF code, closes the file handle, and frees temp memory used by a GIF.
// Many if not most viewers will still display a GIF properly if the EOF code is missing,
// but it's still a good idea to write it out.
bool GIF_Encoder::end()
{
  if(!outFile) return false;

  outFile.write( 0x3b);
  outFile.close();

  GIF_FREE(writerOldImage);

  writerOldImage = NULL;

  return true;
}




