/* stbiw-0.92b - public domain - http://nothings.org/stb/stb_image_write.h
   writes out PNG/BMP/TGA images to C stdio - Sean Barrett 2010
                            no warranty implied; use at your own risk


Before including,

    #define STB_IMAGE_WRITE_IMPLEMENTATION

in the file that you want to have the implementation.


ABOUT:

   This header file is a library for writing images to C stdio. It could be
   adapted to write to memory or a general streaming interface; let me know.

   The PNG output is not optimal; it is 20-50% larger than the file
   written by a decent optimizing implementation. This library is designed
   for source code compactness and simplicitly, not optimal image file size
   or run-time performance.

HISTORY:

   0.92b - added DDS(DXT1/DXT5) from Jonathan Dummer (@rlyeh)

USAGE:

   There are three functions, one for each image file format:

     int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
     int stbi_write_dds(char const *filename, int w, int h, int comp, const void *data);
     int stbi_write_bmp(char const *filename, int w, int h, int comp, const void *data);
     int stbi_write_tga(char const *filename, int w, int h, int comp, const void *data);

   Each function returns 0 on failure and non-0 on success.

   The functions create an image file defined by the parameters. The image
   is a rectangle of pixels stored from left-to-right, top-to-bottom.
   Each pixel contains 'comp' channels of data stored interleaved with 8-bits
   per channel, in the following order: 1=Y, 2=YA, 3=RGB, 4=RGBA. (Y is
   monochrome color.) The rectangle is 'w' pixels wide and 'h' pixels tall.
   The *data pointer points to the first byte of the top-left-most pixel.
   For PNG, "stride_in_bytes" is the distance in bytes from the first byte of
   a row of pixels to the first byte of the next row of pixels.

   PNG creates output files with the same number of components as the input.
   The BMP and TGA formats expand Y to RGB in the file format. BMP does not
   output alpha.

   PNG supports writing rectangles of data even when the bytes storing rows of
   data are not consecutive in memory (e.g. sub-rectangles of a larger image),
   by supplying the stride between the beginning of adjacent rows. The other
   formats do not. (Thus you cannot write a native-format BMP through the BMP
   writer, both because it is in BGR order and because it may have padding
   at the end of the line.)
*/

#ifndef INCLUDE_STB_IMAGE_WRITE_H
#define INCLUDE_STB_IMAGE_WRITE_H

#ifdef __cplusplus
extern "C" {
#endif

extern int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
extern int stbi_write_dds(char const *filename, int w, int h, int comp, const void *data);
extern int stbi_write_bmp(char const *filename, int w, int h, int comp, const void *data);
extern int stbi_write_tga(char const *filename, int w, int h, int comp, const void *data);

#ifdef __cplusplus
}
#endif

#endif//INCLUDE_STB_IMAGE_WRITE_H

#ifdef STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef unsigned int stbiw_uint32;
typedef int stb_image_write_test[sizeof(stbiw_uint32)==4 ? 1 : -1];

static void writefv(FILE *f, const char *fmt, va_list v)
{
   while (*fmt) {
      switch (*fmt++) {
         case ' ': break;
         case '1': { unsigned char x = (unsigned char) va_arg(v, int); fputc(x,f); break; }
         case '2': { int x = va_arg(v,int); unsigned char b[2];
                     b[0] = (unsigned char) x; b[1] = (unsigned char) (x>>8);
                     fwrite(b,2,1,f); break; }
         case '4': { stbiw_uint32 x = va_arg(v,int); unsigned char b[4];
                     b[0]=(unsigned char)x; b[1]=(unsigned char)(x>>8);
                     b[2]=(unsigned char)(x>>16); b[3]=(unsigned char)(x>>24);
                     fwrite(b,4,1,f); break; }
         default:
            assert(0);
            return;
      }
   }
}

static void write3(FILE *f, unsigned char a, unsigned char b, unsigned char c)
{
   unsigned char arr[3];
   arr[0] = a, arr[1] = b, arr[2] = c;
   fwrite(arr, 3, 1, f);
}

static void write_pixels(FILE *f, int rgb_dir, int vdir, int x, int y, int comp, void *data, int write_alpha, int scanline_pad)
{
   unsigned char bg[3] = { 255, 0, 255}, px[3];
   stbiw_uint32 zero = 0;
   int i,j,k, j_end;

   if (y <= 0)
      return;

   if (vdir < 0)
      j_end = -1, j = y-1;
   else
      j_end =  y, j = 0;

   for (; j != j_end; j += vdir) {
      for (i=0; i < x; ++i) {
         unsigned char *d = (unsigned char *) data + (j*x+i)*comp;
         if (write_alpha < 0)
            fwrite(&d[comp-1], 1, 1, f);
         switch (comp) {
            case 1:
            case 2: write3(f, d[0],d[0],d[0]);
                    break;
            case 4:
               if (!write_alpha) {
                  // composite against pink background
                  for (k=0; k < 3; ++k)
                     px[k] = bg[k] + ((d[k] - bg[k]) * d[3])/255;
                  write3(f, px[1-rgb_dir],px[1],px[1+rgb_dir]);
                  break;
               }
               /* FALLTHROUGH */
            case 3:
               write3(f, d[1-rgb_dir],d[1],d[1+rgb_dir]);
               break;
         }
         if (write_alpha > 0)
            fwrite(&d[comp-1], 1, 1, f);
      }
      fwrite(&zero,scanline_pad,1,f);
   }
}

static int outfile(char const *filename, int rgb_dir, int vdir, int x, int y, int comp, void *data, int alpha, int pad, const char *fmt, ...)
{
   FILE *f;
   if (y < 0 || x < 0) return 0;
   f = fopen(filename, "wb");
   if (f) {
      va_list v;
      va_start(v, fmt);
      writefv(f, fmt, v);
      va_end(v);
      write_pixels(f,rgb_dir,vdir,x,y,comp,data,alpha,pad);
      fclose(f);
   }
   return f != NULL;
}

int stbi_write_bmp(char const *filename, int x, int y, int comp, const void *data)
{
   int pad = (-x*3) & 3;
   return outfile(filename,-1,-1,x,y,comp,(void *) data,0,pad,
           "11 4 22 4" "4 44 22 444444",
           'B', 'M', 14+40+(x*3+pad)*y, 0,0, 14+40,  // file header
            40, x,y, 1,24, 0,0,0,0,0,0);             // bitmap header
}

int stbi_write_tga(char const *filename, int x, int y, int comp, const void *data)
{
   int has_alpha = !(comp & 1);
   return outfile(filename, -1,-1, x, y, comp, (void *) data, has_alpha, 0,
                  "111 221 2222 11", 0,0,2, 0,0,0, 0,0,x,y, 24+8*has_alpha, 8*has_alpha);
}

// stretchy buffer; stbi__sbpush() == vector<>::push_back() -- stbi__sbcount() == vector<>::size()
#define stbi__sbraw(a) ((int *) (a) - 2)
#define stbi__sbm(a)   stbi__sbraw(a)[0]
#define stbi__sbn(a)   stbi__sbraw(a)[1]

#define stbi__sbneedgrow(a,n)  ((a)==0 || stbi__sbn(a)+n >= stbi__sbm(a))
#define stbi__sbmaybegrow(a,n) (stbi__sbneedgrow(a,(n)) ? stbi__sbgrow(a,n) : 0)
#define stbi__sbgrow(a,n)  stbi__sbgrowf((void **) &(a), (n), sizeof(*(a)))

#define stbi__sbpush(a, v)      (stbi__sbmaybegrow(a,1), (a)[stbi__sbn(a)++] = (v))
#define stbi__sbcount(a)        ((a) ? stbi__sbn(a) : 0)
#define stbi__sbfree(a)         ((a) ? free(stbi__sbraw(a)),0 : 0)

static void *stbi__sbgrowf(void **arr, int increment, int itemsize)
{
   int m = *arr ? 2*stbi__sbm(*arr)+increment : increment+1;
   void *p = realloc(*arr ? stbi__sbraw(*arr) : 0, itemsize * m + sizeof(int)*2);
   assert(p);
   if (p) {
      if (!*arr) ((int *) p)[1] = 0;
      *arr = (void *) ((int *) p + 2);
      stbi__sbm(*arr) = m;
   }
   return *arr;
}

static unsigned char *stbi__zlib_flushf(unsigned char *data, unsigned int *bitbuffer, int *bitcount)
{
   while (*bitcount >= 8) {
      stbi__sbpush(data, (unsigned char) *bitbuffer);
      *bitbuffer >>= 8;
      *bitcount -= 8;
   }
   return data;
}

static int stbi__zlib_bitrev(int code, int codebits)
{
   int res=0;
   while (codebits--) {
      res = (res << 1) | (code & 1);
      code >>= 1;
   }
   return res;
}

static unsigned int stbi__zlib_countm(unsigned char *a, unsigned char *b, int limit)
{
   int i;
   for (i=0; i < limit && i < 258; ++i)
      if (a[i] != b[i]) break;
   return i;
}

static unsigned int stbi__zhash(unsigned char *data)
{
   stbiw_uint32 hash = data[0] + (data[1] << 8) + (data[2] << 16);
   hash ^= hash << 3;
   hash += hash >> 5;
   hash ^= hash << 4;
   hash += hash >> 17;
   hash ^= hash << 25;
   hash += hash >> 6;
   return hash;
}

#define stbi__zlib_flush() (out = stbi__zlib_flushf(out, &bitbuf, &bitcount))
#define stbi__zlib_add(code,codebits) \
      (bitbuf |= (code) << bitcount, bitcount += (codebits), stbi__zlib_flush())
#define stbi__zlib_huffa(b,c)  stbi__zlib_add(stbi__zlib_bitrev(b,c),c)
// default huffman tables
#define stbi__zlib_huff1(n)  stbi__zlib_huffa(0x30 + (n), 8)
#define stbi__zlib_huff2(n)  stbi__zlib_huffa(0x190 + (n)-144, 9)
#define stbi__zlib_huff3(n)  stbi__zlib_huffa(0 + (n)-256,7)
#define stbi__zlib_huff4(n)  stbi__zlib_huffa(0xc0 + (n)-280,8)
#define stbi__zlib_huff(n)  ((n) <= 143 ? stbi__zlib_huff1(n) : (n) <= 255 ? stbi__zlib_huff2(n) : (n) <= 279 ? stbi__zlib_huff3(n) : stbi__zlib_huff4(n))
#define stbi__zlib_huffb(n) ((n) <= 143 ? stbi__zlib_huff1(n) : stbi__zlib_huff2(n))

#define stbi__ZHASH   16384

unsigned char * stbi_zlib_compress(unsigned char *data, int data_len, int *out_len, int quality)
{
   static unsigned short lengthc[] = { 3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258, 259 };
   static unsigned char  lengtheb[]= { 0,0,0,0,0,0,0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5,  0 };
   static unsigned short distc[]   = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577, 32768 };
   static unsigned char  disteb[]  = { 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13 };
   unsigned int bitbuf=0;
   int i,j, bitcount=0;
   unsigned char *out = NULL;
   unsigned char **hash_table[stbi__ZHASH]; // 64KB on the stack!
   if (quality < 5) quality = 5;

   stbi__sbpush(out, 0x78);   // DEFLATE 32K window
   stbi__sbpush(out, 0x5e);   // FLEVEL = 1
   stbi__zlib_add(1,1);  // BFINAL = 1
   stbi__zlib_add(1,2);  // BTYPE = 1 -- fixed huffman

   for (i=0; i < stbi__ZHASH; ++i)
      hash_table[i] = NULL;

   i=0;
   while (i < data_len-3) {
      // hash next 3 bytes of data to be compressed
      int h = stbi__zhash(data+i)&(stbi__ZHASH-1), best=3;
      unsigned char *bestloc = 0;
      unsigned char **hlist = hash_table[h];
      int n = stbi__sbcount(hlist);
      for (j=0; j < n; ++j) {
         if (hlist[j]-data > i-32768) { // if entry lies within window
            int d = stbi__zlib_countm(hlist[j], data+i, data_len-i);
            if (d >= best) best=d,bestloc=hlist[j];
         }
      }
      // when hash table entry is too long, delete half the entries
      if (hash_table[h] && stbi__sbn(hash_table[h]) == 2*quality) {
         memcpy(hash_table[h], hash_table[h]+quality, sizeof(hash_table[h][0])*quality);
         stbi__sbn(hash_table[h]) = quality;
      }
      stbi__sbpush(hash_table[h],data+i);

      if (bestloc) {
         // "lazy matching" - check match at *next* byte, and if it's better, do cur byte as literal
         h = stbi__zhash(data+i+1)&(stbi__ZHASH-1);
         hlist = hash_table[h];
         n = stbi__sbcount(hlist);
         for (j=0; j < n; ++j) {
            if (hlist[j]-data > i-32767) {
               int e = stbi__zlib_countm(hlist[j], data+i+1, data_len-i-1);
               if (e > best) { // if next match is better, bail on current match
                  bestloc = NULL;
                  break;
               }
            }
         }
      }

      if (bestloc) {
         int d = data+i - bestloc; // distance back
         assert(d <= 32767 && best <= 258);
         for (j=0; best > lengthc[j+1]-1; ++j);
         stbi__zlib_huff(j+257);
         if (lengtheb[j]) stbi__zlib_add(best - lengthc[j], lengtheb[j]);
         for (j=0; d > distc[j+1]-1; ++j);
         stbi__zlib_add(stbi__zlib_bitrev(j,5),5);
         if (disteb[j]) stbi__zlib_add(d - distc[j], disteb[j]);
         i += best;
      } else {
         stbi__zlib_huffb(data[i]);
         ++i;
      }
   }
   // write out final bytes
   for (;i < data_len; ++i)
      stbi__zlib_huffb(data[i]);
   stbi__zlib_huff(256); // end of block
   // pad with 0 bits to byte boundary
   while (bitcount)
      stbi__zlib_add(0,1);

   for (i=0; i < stbi__ZHASH; ++i)
      (void) stbi__sbfree(hash_table[i]);

   {
      // compute adler32 on input
      unsigned int i=0, s1=1, s2=0, blocklen = data_len % 5552;
      int j=0;
      while (j < data_len) {
         for (i=0; i < blocklen; ++i) s1 += data[j+i], s2 += s1;
         s1 %= 65521, s2 %= 65521;
         j += blocklen;
         blocklen = 5552;
      }
      stbi__sbpush(out, (unsigned char) (s2 >> 8));
      stbi__sbpush(out, (unsigned char) s2);
      stbi__sbpush(out, (unsigned char) (s1 >> 8));
      stbi__sbpush(out, (unsigned char) s1);
   }
   *out_len = stbi__sbn(out);
   // make returned pointer freeable
   memmove(stbi__sbraw(out), out, *out_len);
   return (unsigned char *) stbi__sbraw(out);
}

unsigned int stbi__crc32(unsigned char *buffer, int len)
{
   static unsigned int crc_table[256];
   unsigned int crc = ~0u;
   int i,j;
   if (crc_table[1] == 0)
      for(i=0; i < 256; i++)
         for (crc_table[i]=i, j=0; j < 8; ++j)
            crc_table[i] = (crc_table[i] >> 1) ^ (crc_table[i] & 1 ? 0xedb88320 : 0);
   for (i=0; i < len; ++i)
      crc = (crc >> 8) ^ crc_table[buffer[i] ^ (crc & 0xff)];
   return ~crc;
}

#define stbi__wpng4(o,a,b,c,d) ((o)[0]=(unsigned char)(a),(o)[1]=(unsigned char)(b),(o)[2]=(unsigned char)(c),(o)[3]=(unsigned char)(d),(o)+=4)
#define stbi__wp32(data,v) stbi__wpng4(data, (v)>>24,(v)>>16,(v)>>8,(v));
#define stbi__wptag(data,s) stbi__wpng4(data, s[0],s[1],s[2],s[3])

static void stbi__wpcrc(unsigned char **data, int len)
{
   unsigned int crc = stbi__crc32(*data - len - 4, len+4);
   stbi__wp32(*data, crc);
}

static unsigned char stbi__paeth(int a, int b, int c)
{
   int p = a + b - c, pa = abs(p-a), pb = abs(p-b), pc = abs(p-c);
   if (pa <= pb && pa <= pc) return (unsigned char) a;
   if (pb <= pc) return (unsigned char) b;
   return (unsigned char) c;
}

unsigned char *stbi_write_png_to_mem(unsigned char *pixels, int stride_bytes, int x, int y, int n, int *out_len)
{
   int ctype[5] = { -1, 0, 4, 2, 6 };
   unsigned char sig[8] = { 137,80,78,71,13,10,26,10 };
   unsigned char *out,*o, *filt, *zlib;
   signed char *line_buffer;
   int i,j,k,p,zlen;

   if (stride_bytes == 0)
      stride_bytes = x * n;

   filt = (unsigned char *) malloc((x*n+1) * y); if (!filt) return 0;
   line_buffer = (signed char *) malloc(x * n); if (!line_buffer) { free(filt); return 0; }
   for (j=0; j < y; ++j) {
      static int mapping[] = { 0,1,2,3,4 };
      static int firstmap[] = { 0,1,0,5,6 };
      int *mymap = j ? mapping : firstmap;
      int best = 0, bestval = 0x7fffffff;
      for (p=0; p < 2; ++p) {
         for (k= p?best:0; k < 5; ++k) {
            int type = mymap[k],est=0;
            unsigned char *z = pixels + stride_bytes*j;
            for (i=0; i < n; ++i)
               switch (type) {
                  case 0: line_buffer[i] = z[i]; break;
                  case 1: line_buffer[i] = z[i]; break;
                  case 2: line_buffer[i] = z[i] - z[i-stride_bytes]; break;
                  case 3: line_buffer[i] = z[i] - (z[i-stride_bytes]>>1); break;
                  case 4: line_buffer[i] = (signed char) (z[i] - stbi__paeth(0,z[i-stride_bytes],0)); break;
                  case 5: line_buffer[i] = z[i]; break;
                  case 6: line_buffer[i] = z[i]; break;
               }
            for (i=n; i < x*n; ++i) {
               switch (type) {
                  case 0: line_buffer[i] = z[i]; break;
                  case 1: line_buffer[i] = z[i] - z[i-n]; break;
                  case 2: line_buffer[i] = z[i] - z[i-stride_bytes]; break;
                  case 3: line_buffer[i] = z[i] - ((z[i-n] + z[i-stride_bytes])>>1); break;
                  case 4: line_buffer[i] = z[i] - stbi__paeth(z[i-n], z[i-stride_bytes], z[i-stride_bytes-n]); break;
                  case 5: line_buffer[i] = z[i] - (z[i-n]>>1); break;
                  case 6: line_buffer[i] = z[i] - stbi__paeth(z[i-n], 0,0); break;
               }
            }
            if (p) break;
            for (i=0; i < x*n; ++i)
               est += abs((signed char) line_buffer[i]);
            if (est < bestval) { bestval = est; best = k; }
         }
      }
      // when we get here, best contains the filter type, and line_buffer contains the data
      filt[j*(x*n+1)] = (unsigned char) best;
      memcpy(filt+j*(x*n+1)+1, line_buffer, x*n);
   }
   free(line_buffer);
   zlib = stbi_zlib_compress(filt, y*( x*n+1), &zlen, 8); // increase 8 to get smaller but use more memory
   free(filt);
   if (!zlib) return 0;

   // each tag requires 12 bytes of overhead
   out = (unsigned char *) malloc(8 + 12+13 + 12+zlen + 12);
   if (!out) return 0;
   *out_len = 8 + 12+13 + 12+zlen + 12;

   o=out;
   memcpy(o,sig,8); o+= 8;
   stbi__wp32(o, 13); // header length
   stbi__wptag(o, "IHDR");
   stbi__wp32(o, x);
   stbi__wp32(o, y);
   *o++ = 8;
   *o++ = (unsigned char) ctype[n];
   *o++ = 0;
   *o++ = 0;
   *o++ = 0;
   stbi__wpcrc(&o,13);

   stbi__wp32(o, zlen);
   stbi__wptag(o, "IDAT");
   memcpy(o, zlib, zlen); o += zlen; free(zlib);
   stbi__wpcrc(&o, zlen);

   stbi__wp32(o,0);
   stbi__wptag(o, "IEND");
   stbi__wpcrc(&o,0);

   assert(o == out + *out_len);

   return out;
}

int stbi_write_png(char const *filename, int x, int y, int comp, const void *data, int stride_bytes)
{
   FILE *f;
   int len;
   unsigned char *png = stbi_write_png_to_mem((unsigned char *) data, stride_bytes, x, y, comp, &len);
   if (!png) return 0;
   f = fopen(filename, "wb");
   if (!f) { free(png); return 0; }
   fwrite(png, 1, len, f);
   fclose(f);
   free(png);
   return 1;
}

/* image_DXT.h following (@rlyeh) */

/*
   Jonathan Dummer
   2007-07-31-10.32

   simple DXT compression / decompression code

   public domain
*/

#ifndef HEADER_IMAGE_DXT
#define HEADER_IMAGE_DXT

/**
   Converts an image from an array of unsigned chars (RGB or RGBA) to
   DXT1 or DXT5, then saves the converted image to disk.
   \return 0 if failed, otherwise returns 1
**/
int
save_image_as_DDS
(
    const char *filename,
    int width, int height, int channels,
    const unsigned char *const data
);

/**
   take an image and convert it to DXT1 (no alpha)
**/
unsigned char*
convert_image_to_DXT1
(
    const unsigned char *const uncompressed,
    int width, int height, int channels,
    int *out_size
);

/**
   take an image and convert it to DXT5 (with alpha)
**/
unsigned char*
convert_image_to_DXT5
(
    const unsigned char *const uncompressed,
    int width, int height, int channels,
    int *out_size
);

/**   A bunch of DirectDraw Surface structures and flags **/
typedef struct
{
    unsigned int    dwMagic;
    unsigned int    dwSize;
    unsigned int    dwFlags;
    unsigned int    dwHeight;
    unsigned int    dwWidth;
    unsigned int    dwPitchOrLinearSize;
    unsigned int    dwDepth;
    unsigned int    dwMipMapCount;
    unsigned int    dwReserved1[ 11 ];

    /*  DDPIXELFORMAT   */
    struct
    {
        unsigned int    dwSize;
        unsigned int    dwFlags;
        unsigned int    dwFourCC;
        unsigned int    dwRGBBitCount;
        unsigned int    dwRBitMask;
        unsigned int    dwGBitMask;
        unsigned int    dwBBitMask;
        unsigned int    dwAlphaBitMask;
    }
    sPixelFormat;

    /*  DDCAPS2   */
    struct
    {
        unsigned int    dwCaps1;
        unsigned int    dwCaps2;
        unsigned int    dwDDSX;
        unsigned int    dwReserved;
    }
    sCaps;
    unsigned int    dwReserved2;
}
DDS_header ;

/* the following constants were copied directly off the MSDN website */

/* The dwFlags member of the original DDSURFACEDESC2 structure
   can be set to one or more of the following values. */
#define DDSD_CAPS 0x00000001
#define DDSD_HEIGHT  0x00000002
#define DDSD_WIDTH   0x00000004
#define DDSD_PITCH   0x00000008
#define DDSD_PIXELFORMAT   0x00001000
#define DDSD_MIPMAPCOUNT   0x00020000
#define DDSD_LINEARSIZE 0x00080000
#define DDSD_DEPTH   0x00800000

/* DirectDraw Pixel Format */
#define DDPF_ALPHAPIXELS   0x00000001
#define DDPF_FOURCC  0x00000004
#define DDPF_RGB  0x00000040

/* The dwCaps1 member of the DDSCAPS2 structure can be
   set to one or more of the following values.  */
#define DDSCAPS_COMPLEX 0x00000008
#define DDSCAPS_TEXTURE 0x00001000
#define DDSCAPS_MIPMAP  0x00400000

/* The dwCaps2 member of the DDSCAPS2 structure can be
   set to one or more of the following values.     */
#define DDSCAPS2_CUBEMAP   0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000
#define DDSCAPS2_VOLUME 0x00200000

#endif /* HEADER_IMAGE_DXT */

/* end of image_DXT.h (@rlyeh) */

/* image_DXT.c following (@rlyeh) */

/*
   Jonathan Dummer
   2007-07-31-10.32

   simple DXT compression / decompression code

   public domain
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* set this =1 if you want to use the covarince matrix method...
   which is better than my method of using standard deviations
   overall, except on the infintesimal chance that the power
   method fails for finding the largest eigenvector   */
#define USE_COV_MAT  1

/********* Function Prototypes *********/
/*
   Takes a 4x4 block of pixels and compresses it into 8 bytes
   in DXT1 format (color only, no alpha).  Speed is valued
   over prettyness, at least for now.
*/
void compress_DDS_color_block(
            int channels,
            const unsigned char *const uncompressed,
            unsigned char compressed[8] );
/*
   Takes a 4x4 block of pixels and compresses the alpha
   component it into 8 bytes for use in DXT5 DDS files.
   Speed is valued over prettyness, at least for now.
*/
void compress_DDS_alpha_block(
            const unsigned char *const uncompressed,
            unsigned char compressed[8] );

/********* Actual Exposed Functions *********/
int
   save_image_as_DDS
   (
      const char *filename,
      int width, int height, int channels,
      const unsigned char *const data
   )
{
   /* variables   */
   FILE *fout;
   unsigned char *DDS_data;
   DDS_header header;
   int DDS_size;
   /* error check */
   if( (NULL == filename) ||
      (width < 1) || (height < 1) ||
      (channels < 1) || (channels > 4) ||
      (data == NULL ) )
   {
      return 0;
   }
   /* Convert the image */
   if( (channels & 1) == 1 )
   {
      /* no alpha, just use DXT1 */
      DDS_data = convert_image_to_DXT1( data, width, height, channels, &DDS_size );
   } else
   {
      /* has alpha, so use DXT5  */
      DDS_data = convert_image_to_DXT5( data, width, height, channels, &DDS_size );
   }
   /* save it  */
   memset( &header, 0, sizeof( DDS_header ) );
   header.dwMagic = ('D' << 0) | ('D' << 8) | ('S' << 16) | (' ' << 24);
   header.dwSize = 124;
   header.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_LINEARSIZE;
   header.dwWidth = width;
   header.dwHeight = height;
   header.dwPitchOrLinearSize = DDS_size;
   header.sPixelFormat.dwSize = 32;
   header.sPixelFormat.dwFlags = DDPF_FOURCC;
   if( (channels & 1) == 1 )
   {
      header.sPixelFormat.dwFourCC = ('D' << 0) | ('X' << 8) | ('T' << 16) | ('1' << 24);
   } else
   {
      header.sPixelFormat.dwFourCC = ('D' << 0) | ('X' << 8) | ('T' << 16) | ('5' << 24);
   }
   header.sCaps.dwCaps1 = DDSCAPS_TEXTURE;
   /* write it out   */
   fout = fopen( filename, "wb");
   fwrite( &header, sizeof( DDS_header ), 1, fout );
   fwrite( DDS_data, 1, DDS_size, fout );
   fclose( fout );
   /* done  */
   free( DDS_data );
   return 1;
}

unsigned char* convert_image_to_DXT1(
      const unsigned char *const uncompressed,
      int width, int height, int channels,
      int *out_size )
{
   unsigned char *compressed;
   int i, j, x, y;
   unsigned char ublock[16*3];
   unsigned char cblock[8];
   int index = 0, chan_step = 1;
   int block_count = 0;
   /* error check */
   *out_size = 0;
   if( (width < 1) || (height < 1) ||
      (NULL == uncompressed) ||
      (channels < 1) || (channels > 4) )
   {
      return NULL;
   }
   /* for channels == 1 or 2, I do not step forward for R,G,B values */
   if( channels < 3 )
   {
      chan_step = 0;
   }
   /* get the RAM for the compressed image
      (8 bytes per 4x4 pixel block) */
   *out_size = ((width+3) >> 2) * ((height+3) >> 2) * 8;
   compressed = (unsigned char*)malloc( *out_size );
   /* go through each block   */
   for( j = 0; j < height; j += 4 )
   {
      for( i = 0; i < width; i += 4 )
      {
         /* copy this block into a new one   */
         int idx = 0;
         int mx = 4, my = 4;
         if( j+4 >= height )
         {
            my = height - j;
         }
         if( i+4 >= width )
         {
            mx = width - i;
         }
         for( y = 0; y < my; ++y )
         {
            for( x = 0; x < mx; ++x )
            {
               ublock[idx++] = uncompressed[(j+y)*width*channels+(i+x)*channels];
               ublock[idx++] = uncompressed[(j+y)*width*channels+(i+x)*channels+chan_step];
               ublock[idx++] = uncompressed[(j+y)*width*channels+(i+x)*channels+chan_step+chan_step];
            }
            for( x = mx; x < 4; ++x )
            {
               ublock[idx++] = ublock[0];
               ublock[idx++] = ublock[1];
               ublock[idx++] = ublock[2];
            }
         }
         for( y = my; y < 4; ++y )
         {
            for( x = 0; x < 4; ++x )
            {
               ublock[idx++] = ublock[0];
               ublock[idx++] = ublock[1];
               ublock[idx++] = ublock[2];
            }
         }
         /* compress the block   */
         ++block_count;
         compress_DDS_color_block( 3, ublock, cblock );
         /* copy the data from the block into the main block   */
         for( x = 0; x < 8; ++x )
         {
            compressed[index++] = cblock[x];
         }
      }
   }
   return compressed;
}

unsigned char* convert_image_to_DXT5(
      const unsigned char *const uncompressed,
      int width, int height, int channels,
      int *out_size )
{
   unsigned char *compressed;
   int i, j, x, y;
   unsigned char ublock[16*4];
   unsigned char cblock[8];
   int index = 0, chan_step = 1;
   int block_count = 0, has_alpha;
   /* error check */
   *out_size = 0;
   if( (width < 1) || (height < 1) ||
      (NULL == uncompressed) ||
      (channels < 1) || ( channels > 4) )
   {
      return NULL;
   }
   /* for channels == 1 or 2, I do not step forward for R,G,B vales  */
   if( channels < 3 )
   {
      chan_step = 0;
   }
   /* # channels = 1 or 3 have no alpha, 2 & 4 do have alpha   */
   has_alpha = 1 - (channels & 1);
   /* get the RAM for the compressed image
      (16 bytes per 4x4 pixel block)   */
   *out_size = ((width+3) >> 2) * ((height+3) >> 2) * 16;
   compressed = (unsigned char*)malloc( *out_size );
   /* go through each block   */
   for( j = 0; j < height; j += 4 )
   {
      for( i = 0; i < width; i += 4 )
      {
         /* local variables, and my block counter  */
         int idx = 0;
         int mx = 4, my = 4;
         if( j+4 >= height )
         {
            my = height - j;
         }
         if( i+4 >= width )
         {
            mx = width - i;
         }
         for( y = 0; y < my; ++y )
         {
            for( x = 0; x < mx; ++x )
            {
               ublock[idx++] = uncompressed[(j+y)*width*channels+(i+x)*channels];
               ublock[idx++] = uncompressed[(j+y)*width*channels+(i+x)*channels+chan_step];
               ublock[idx++] = uncompressed[(j+y)*width*channels+(i+x)*channels+chan_step+chan_step];
               ublock[idx++] =
                  has_alpha * uncompressed[(j+y)*width*channels+(i+x)*channels+channels-1]
                  + (1-has_alpha)*255;
            }
            for( x = mx; x < 4; ++x )
            {
               ublock[idx++] = ublock[0];
               ublock[idx++] = ublock[1];
               ublock[idx++] = ublock[2];
               ublock[idx++] = ublock[3];
            }
         }
         for( y = my; y < 4; ++y )
         {
            for( x = 0; x < 4; ++x )
            {
               ublock[idx++] = ublock[0];
               ublock[idx++] = ublock[1];
               ublock[idx++] = ublock[2];
               ublock[idx++] = ublock[3];
            }
         }
         /* now compress the alpha block  */
         compress_DDS_alpha_block( ublock, cblock );
         /* copy the data from the compressed alpha block into the main buffer   */
         for( x = 0; x < 8; ++x )
         {
            compressed[index++] = cblock[x];
         }
         /* then compress the color block */
         ++block_count;
         compress_DDS_color_block( 4, ublock, cblock );
         /* copy the data from the compressed color block into the main buffer   */
         for( x = 0; x < 8; ++x )
         {
            compressed[index++] = cblock[x];
         }
      }
   }
   return compressed;
}

/********* Helper Functions *********/
int convert_bit_range( int c, int from_bits, int to_bits )
{
   int b = (1 << (from_bits - 1)) + c * ((1 << to_bits) - 1);
   return (b + (b >> from_bits)) >> from_bits;
}

int rgb_to_565( int r, int g, int b )
{
   return
      (convert_bit_range( r, 8, 5 ) << 11) |
      (convert_bit_range( g, 8, 6 ) << 05) |
      (convert_bit_range( b, 8, 5 ) << 00);
}

void rgb_888_from_565( unsigned int c, int *r, int *g, int *b )
{
   *r = convert_bit_range( (c >> 11) & 31, 5, 8 );
   *g = convert_bit_range( (c >> 05) & 63, 6, 8 );
   *b = convert_bit_range( (c >> 00) & 31, 5, 8 );
}

void compute_color_line_STDEV(
      const unsigned char *const uncompressed,
      int channels,
      float point[3], float direction[3] )
{
   const float inv_16 = 1.0f / 16.0f;
   int i;
   float sum_r = 0.0f, sum_g = 0.0f, sum_b = 0.0f;
   float sum_rr = 0.0f, sum_gg = 0.0f, sum_bb = 0.0f;
   float sum_rg = 0.0f, sum_rb = 0.0f, sum_gb = 0.0f;
   /* calculate all data needed for the covariance matrix
      ( to compare with _rygdxt code)  */
   for( i = 0; i < 16*channels; i += channels )
   {
      sum_r += uncompressed[i+0];
      sum_rr += uncompressed[i+0] * uncompressed[i+0];
      sum_g += uncompressed[i+1];
      sum_gg += uncompressed[i+1] * uncompressed[i+1];
      sum_b += uncompressed[i+2];
      sum_bb += uncompressed[i+2] * uncompressed[i+2];
      sum_rg += uncompressed[i+0] * uncompressed[i+1];
      sum_rb += uncompressed[i+0] * uncompressed[i+2];
      sum_gb += uncompressed[i+1] * uncompressed[i+2];
   }
   /* convert the sums to averages  */
   sum_r *= inv_16;
   sum_g *= inv_16;
   sum_b *= inv_16;
   /* and convert the squares to the squares of the value - avg_value   */
   sum_rr -= 16.0f * sum_r * sum_r;
   sum_gg -= 16.0f * sum_g * sum_g;
   sum_bb -= 16.0f * sum_b * sum_b;
   sum_rg -= 16.0f * sum_r * sum_g;
   sum_rb -= 16.0f * sum_r * sum_b;
   sum_gb -= 16.0f * sum_g * sum_b;
   /* the point on the color line is the average   */
   point[0] = sum_r;
   point[1] = sum_g;
   point[2] = sum_b;
   #if USE_COV_MAT
   /*
      The following idea was from ryg.
      (https://mollyrocket.com/forums/viewtopic.php?t=392)
      The method worked great (less RMSE than mine) most of
      the time, but had some issues handling some simple
      boundary cases, like full green next to full red,
      which would generate a covariance matrix like this:

      | 1  -1  0 |
      | -1  1  0 |
      | 0   0  0 |

      For a given starting vector, the power method can
      generate all zeros!  So no starting with {1,1,1}
      as I was doing!  This kind of error is still a
      slight posibillity, but will be very rare.
   */
   /* use the covariance matrix directly
      (1st iteration, don't use all 1.0 values!)   */
   sum_r = 1.0f;
   sum_g = 2.718281828f;
   sum_b = 3.141592654f;
   direction[0] = sum_r*sum_rr + sum_g*sum_rg + sum_b*sum_rb;
   direction[1] = sum_r*sum_rg + sum_g*sum_gg + sum_b*sum_gb;
   direction[2] = sum_r*sum_rb + sum_g*sum_gb + sum_b*sum_bb;
   /* 2nd iteration, use results from the 1st guy  */
   sum_r = direction[0];
   sum_g = direction[1];
   sum_b = direction[2];
   direction[0] = sum_r*sum_rr + sum_g*sum_rg + sum_b*sum_rb;
   direction[1] = sum_r*sum_rg + sum_g*sum_gg + sum_b*sum_gb;
   direction[2] = sum_r*sum_rb + sum_g*sum_gb + sum_b*sum_bb;
   /* 3rd iteration, use results from the 2nd guy  */
   sum_r = direction[0];
   sum_g = direction[1];
   sum_b = direction[2];
   direction[0] = sum_r*sum_rr + sum_g*sum_rg + sum_b*sum_rb;
   direction[1] = sum_r*sum_rg + sum_g*sum_gg + sum_b*sum_gb;
   direction[2] = sum_r*sum_rb + sum_g*sum_gb + sum_b*sum_bb;
   #else
   /* use my standard deviation method
      (very robust, a tiny bit slower and less accurate) */
   direction[0] = sqrt( sum_rr );
   direction[1] = sqrt( sum_gg );
   direction[2] = sqrt( sum_bb );
   /* which has a greater component */
   if( sum_gg > sum_rr )
   {
      /* green has greater component, so base the other signs off of green */
      if( sum_rg < 0.0f )
      {
         direction[0] = -direction[0];
      }
      if( sum_gb < 0.0f )
      {
         direction[2] = -direction[2];
      }
   } else
   {
      /* red has a greater component   */
      if( sum_rg < 0.0f )
      {
         direction[1] = -direction[1];
      }
      if( sum_rb < 0.0f )
      {
         direction[2] = -direction[2];
      }
   }
   #endif
}

void LSE_master_colors_max_min(
      int *cmax, int *cmin,
      int channels,
      const unsigned char *const uncompressed )
{
   int i, j;
   /* the master colors */
   int c0[3], c1[3];
   /* used for fitting the line  */
   float sum_x[] = { 0.0f, 0.0f, 0.0f };
   float sum_x2[] = { 0.0f, 0.0f, 0.0f };
   float dot_max = 1.0f, dot_min = -1.0f;
   float vec_len2 = 0.0f;
   float dot;
   /* error check */
   if( (channels < 3) || (channels > 4) )
   {
      return;
   }
   compute_color_line_STDEV( uncompressed, channels, sum_x, sum_x2 );
   vec_len2 = 1.0f / ( 0.00001f +
         sum_x2[0]*sum_x2[0] + sum_x2[1]*sum_x2[1] + sum_x2[2]*sum_x2[2] );
   /* finding the max and min vector values  */
   dot_max =
         (
            sum_x2[0] * uncompressed[0] +
            sum_x2[1] * uncompressed[1] +
            sum_x2[2] * uncompressed[2]
         );
   dot_min = dot_max;
   for( i = 1; i < 16; ++i )
   {
      dot =
         (
            sum_x2[0] * uncompressed[i*channels+0] +
            sum_x2[1] * uncompressed[i*channels+1] +
            sum_x2[2] * uncompressed[i*channels+2]
         );
      if( dot < dot_min )
      {
         dot_min = dot;
      } else if( dot > dot_max )
      {
         dot_max = dot;
      }
   }
   /* and the offset (from the average location)   */
   dot = sum_x2[0]*sum_x[0] + sum_x2[1]*sum_x[1] + sum_x2[2]*sum_x[2];
   dot_min -= dot;
   dot_max -= dot;
   /* post multiply by the scaling factor */
   dot_min *= vec_len2;
   dot_max *= vec_len2;
   /* OK, build the master colors   */
   for( i = 0; i < 3; ++i )
   {
      /* color 0  */
      c0[i] = (int)(0.5f + sum_x[i] + dot_max * sum_x2[i]);
      if( c0[i] < 0 )
      {
         c0[i] = 0;
      } else if( c0[i] > 255 )
      {
         c0[i] = 255;
      }
      /* color 1  */
      c1[i] = (int)(0.5f + sum_x[i] + dot_min * sum_x2[i]);
      if( c1[i] < 0 )
      {
         c1[i] = 0;
      } else if( c1[i] > 255 )
      {
         c1[i] = 255;
      }
   }
   /* down_sample (with rounding?)  */
   i = rgb_to_565( c0[0], c0[1], c0[2] );
   j = rgb_to_565( c1[0], c1[1], c1[2] );
   if( i > j )
   {
      *cmax = i;
      *cmin = j;
   } else
   {
      *cmax = j;
      *cmin = i;
   }
}

void
   compress_DDS_color_block
   (
      int channels,
      const unsigned char *const uncompressed,
      unsigned char compressed[8]
   )
{
   /* variables   */
   int i;
   int next_bit;
   int enc_c0, enc_c1;
   int c0[4], c1[4];
   float color_line[] = { 0.0f, 0.0f, 0.0f, 0.0f };
   float vec_len2 = 0.0f, dot_offset = 0.0f;
   /* stupid order   */
   int swizzle4[] = { 0, 2, 3, 1 };
   /* get the master colors   */
   LSE_master_colors_max_min( &enc_c0, &enc_c1, channels, uncompressed );
   /* store the 565 color 0 and color 1   */
   compressed[0] = (enc_c0 >> 0) & 255;
   compressed[1] = (enc_c0 >> 8) & 255;
   compressed[2] = (enc_c1 >> 0) & 255;
   compressed[3] = (enc_c1 >> 8) & 255;
   /* zero out the compressed data  */
   compressed[4] = 0;
   compressed[5] = 0;
   compressed[6] = 0;
   compressed[7] = 0;
   /* reconstitute the master color vectors  */
   rgb_888_from_565( enc_c0, &c0[0], &c0[1], &c0[2] );
   rgb_888_from_565( enc_c1, &c1[0], &c1[1], &c1[2] );
   /* the new vector */
   vec_len2 = 0.0f;
   for( i = 0; i < 3; ++i )
   {
      color_line[i] = (float)(c1[i] - c0[i]);
      vec_len2 += color_line[i] * color_line[i];
   }
   if( vec_len2 > 0.0f )
   {
      vec_len2 = 1.0f / vec_len2;
   }
   /* pre-proform the scaling */
   color_line[0] *= vec_len2;
   color_line[1] *= vec_len2;
   color_line[2] *= vec_len2;
   /* compute the offset (constant) portion of the dot product */
   dot_offset = color_line[0]*c0[0] + color_line[1]*c0[1] + color_line[2]*c0[2];
   /* store the rest of the bits */
   next_bit = 8*4;
   for( i = 0; i < 16; ++i )
   {
      /* find the dot product of this color, to place it on the line
         (should be [-1,1])   */
      int next_value = 0;
      float dot_product =
         color_line[0] * uncompressed[i*channels+0] +
         color_line[1] * uncompressed[i*channels+1] +
         color_line[2] * uncompressed[i*channels+2] -
         dot_offset;
      /* map to [0,3]   */
      next_value = (int)( dot_product * 3.0f + 0.5f );
      if( next_value > 3 )
      {
         next_value = 3;
      } else if( next_value < 0 )
      {
         next_value = 0;
      }
      /* OK, store this value */
      compressed[next_bit >> 3] |= swizzle4[ next_value ] << (next_bit & 7);
      next_bit += 2;
   }
   /* done compressing to DXT1   */
}

void
   compress_DDS_alpha_block
   (
      const unsigned char *const uncompressed,
      unsigned char compressed[8]
   )
{
   /* variables   */
   int i;
   int next_bit;
   int a0, a1;
   float scale_me;
   /* stupid order   */
   int swizzle8[] = { 1, 7, 6, 5, 4, 3, 2, 0 };
   /* get the alpha limits (a0 > a1)   */
   a0 = a1 = uncompressed[3];
   for( i = 4+3; i < 16*4; i += 4 )
   {
      if( uncompressed[i] > a0 )
      {
         a0 = uncompressed[i];
      } else if( uncompressed[i] < a1 )
      {
         a1 = uncompressed[i];
      }
   }
   /* store those limits, and zero the rest of the compressed dataset   */
   compressed[0] = a0;
   compressed[1] = a1;
   /* zero out the compressed data  */
   compressed[2] = 0;
   compressed[3] = 0;
   compressed[4] = 0;
   compressed[5] = 0;
   compressed[6] = 0;
   compressed[7] = 0;
   /* store the all of the alpha values   */
   next_bit = 8*2;
   scale_me = 7.9999f / (a0 - a1);
   for( i = 3; i < 16*4; i += 4 )
   {
      /* convert this alpha value to a 3 bit number   */
      int svalue;
      int value = (int)((uncompressed[i] - a1) * scale_me);
      svalue = swizzle8[ value&7 ];
      /* OK, store this value, start with the 1st byte   */
      compressed[next_bit >> 3] |= svalue << (next_bit & 7);
      if( (next_bit & 7) > 5 )
      {
         /* spans 2 bytes, fill in the start of the 2nd byte   */
         compressed[1 + (next_bit >> 3)] |= svalue >> (8 - (next_bit & 7) );
      }
      next_bit += 3;
   }
   /* done compressing to DXT1   */
}

/* end of image_DXT.c (@rlyeh) */

int stbi_write_dds( char const *filename, int w, int h, int comp, const void *data )
{
   return save_image_as_DDS( filename, w, h, comp, data );
}


#endif // STB_IMAGE_WRITE_IMPLEMENTATION

/* Revision history

      0.92 (2010-08-01)
             casts to unsigned char to fix warnings
      0.91 (2010-07-17)
             first public release
      0.90   first internal release
*/
