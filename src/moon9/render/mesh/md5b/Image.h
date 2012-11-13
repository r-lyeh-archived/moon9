/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Image.h -- Copyright (c) 2006 David Henry
// last modification: feb. 25, 2006
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Declaration of DDS, TGA, PCX, JPEG and PNG image loader classes.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "../render.hpp"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

using std::cout;
using std::endl;
using std::string;

#if (__cplusplus > 199711) || (defined(_MSC_VER) && _MSC_VER >= 1600)
using std::shared_ptr;
#else
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
#endif

/////////////////////////////////////////////////////////////////////////////
// Image class diagram:
//
//    +------- (abs)                     +---------------+
//    |  Image  |                        | runtime_error |
//    +---------+                        +---------------+
//         ^                                     ^
//         |   +------------+                    |
//         +---|  ImageDDS  |            +----------------+
//         |   +------------+            | ImageException |
//         |                             +----------------+
//         |   +------------+
//         +---|  ImageTGA  |
//         |   +------------+
//         |                             +---------------+
//         |   +------------+            |  ImageBuffer  |
//         +---|  ImagePCX  |            +---------------+
//         |   +------------+
//         |
//         |   +------------+
//         +---|  ImageJPEG |            +----------------+
//         |   +------------+            |  ImageFactory  |
//         |                             +----------------+
//         |   +------------+
//         +---|  ImagePNG  |
//             +------------+
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
// class ImageException - Exception class for ImageBuffer and Image
// loaders.  This acts like a standard runtime_error exception but
// know which file has failed to be loaded.
//
/////////////////////////////////////////////////////////////////////////////

class ImageException : public std::runtime_error
{
public:
  // Constructors
  ImageException (const string &error)
    : std::runtime_error (error) { }
  ImageException (const string &error, const string &filename)
    : std::runtime_error (error), _which (filename) { }
  virtual ~ImageException () throw () { }

public:
  // Public interface
  virtual const char *which () const throw () {
    return _which.c_str ();
  }

private:
  // Member variables
  string _which;
};


/////////////////////////////////////////////////////////////////////////////
//
// class ImageBuffer - An image file loader class.  Load a whole file
// into a memory buffer.
//
/////////////////////////////////////////////////////////////////////////////

class ImageBuffer
{
public:
  // Constructors/destructor
  ImageBuffer (const string &filename);

  ImageBuffer (const ImageBuffer &that)
    : _filename (that._filename), _data (NULL), _length (that._length)
  {
    _data = new GLubyte[_length];
    memcpy (_data, that._data, _length);
  }

  ~ImageBuffer ();

private:
  // Disable default constructor
  ImageBuffer ();

public:
  // Accessors
  const string &filename () const { return _filename; }
  const GLubyte *data () const { return _data; }
  size_t length () const { return _length; }

  ImageBuffer &operator= (const ImageBuffer &rhs)
  {
    this->~ImageBuffer ();
    new (this) ImageBuffer (rhs);
    return *this;
  }

private:
  // Member variables
  string _filename;

  GLubyte *_data;
  size_t _length;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Image - A generic image loader class for creating OpenGL
// textures from.  All other specific image loader are derived from it.
//
/////////////////////////////////////////////////////////////////////////////

class Image
{
protected:
  // Default constructor
  Image ()
    : _width (0), _height (0), _numMipmaps (0),
      _format (0), _components (0), _pixels (NULL),
      _standardCoordSystem (true) { }

private:
  // Disable copy constructor.
  Image (const Image &img);

public:
  // Constructors/destructor
  Image (const string &name, GLsizei w, GLsizei h, GLint numMipMaps,
	 GLenum format, GLint components, const GLubyte *pixels,
	 bool stdCoordSystem);

  virtual ~Image();

public:
  // Return true if we're working with S3
  // compressed textures (DDS files)
  bool isCompressed () const;
  bool isPowerOfTwo () const;

  // Accessors
  GLsizei width () const { return _width; }
  GLsizei height () const { return _height; }
  GLint numMipmaps () const { return _numMipmaps; }
  GLenum format () const { return _format; }
  GLint components () const { return _components; }
  const GLubyte *pixels () const { return _pixels; }
  const string &name () const { return _name; }
  bool stdCoordSystem () const { return _standardCoordSystem; }

protected:
  // Member variables
  GLsizei _width;
  GLsizei _height;
  GLint _numMipmaps;

  // OpenGL texture format and internal
  // format (components)
  GLenum _format;
  GLint _components;

  // Image data
  GLubyte *_pixels;

  string _name;

  // Is the picture in standard OpenGL 2D coordinate
  // system? (starts lower-left corner)
  bool _standardCoordSystem;
};


// Definition of type aliases
typedef shared_ptr<Image> ImagePtr;



/////////////////////////////////////////////////////////////////////////////
//
// class glImageTGA - A TrueVision TARGA (TGA) image loader class.
// Support 24-32 bits BGR files; 16 bits RGB; 8 bits indexed (BGR
// palette); 8 and 16 bits grayscale; all compressed and uncompressed.
// Compressed TGA images use RLE algorithm.
//
/////////////////////////////////////////////////////////////////////////////

class ImageTGA : public Image
{
public:
  // Constructor
  ImageTGA (const ImageBuffer &ibuff);

private:
  // Internal functions
  void getTextureInfo ();

  void readTGA8bits (const GLubyte *data, const GLubyte *colormap);
  void readTGA16bits (const GLubyte *data);
  void readTGA24bits (const GLubyte *data);
  void readTGA32bits (const GLubyte *data);
  void readTGAgray8bits (const GLubyte *data);
  void readTGAgray16bits (const GLubyte *data);

  void readTGA8bitsRLE (const GLubyte *data, const GLubyte *colormap);
  void readTGA16bitsRLE (const GLubyte *data);
  void readTGA24bitsRLE (const GLubyte *data);
  void readTGA32bitsRLE (const GLubyte *data);
  void readTGAgray8bitsRLE (const GLubyte *data);
  void readTGAgray16bitsRLE (const GLubyte *data);

private:
  // Member variables
#pragma pack(push, 1)
  // tga header
  struct TGA_Header
  {
    GLubyte id_lenght;        // size of image id
    GLubyte colormap_type;    // 1 is has a colormap
    GLubyte image_type;       // compression type

    short cm_first_entry;     // colormap origin
    short cm_length;          // colormap length
    GLubyte cm_size;          // colormap size

    short x_origin;           // bottom left x coord origin
    short y_origin;           // bottom left y coord origin

    short width;              // picture width (in pixels)
    short height;             // picture height (in pixels)

    GLubyte pixel_depth;      // bits per pixel: 8, 16, 24 or 32
    GLubyte image_descriptor; // 24 bits = 0x00; 32 bits = 0x80

  };
#pragma pack(pop)

  const TGA_Header *_header;

  // NOTE:
  // 16 bits images are stored in RGB
  // 8-24-32 images are stored in BGR(A)

  // RGBA/BGRA component table access -- usefull for
  // switching from bgra to rgba at load time.
  static int rgbaTable[4]; // bgra to rgba: 2, 1, 0, 3
  static int bgraTable[4]; // bgra to bgra: 0, 1, 2, 3
};


/////////////////////////////////////////////////////////////////////////////
//
// class ImageFactory - An Image Factory Class.
//
/////////////////////////////////////////////////////////////////////////////

class ImageFactory
{
public:
  // Public interface
  static Image *createImage (const ImageBuffer &ibuff)
  {
    string ext;
    Image *result;

    // Extract file extension
    const string &filename = ibuff.filename ();
    ext.assign (filename, filename.find_last_of ('.') + 1, string::npos);

    if (ext.compare ("tga") == 0)
      {
	result = new ImageTGA (ibuff);
      }
    else
      {
	throw ImageException ("Unhandled image file format", filename);
      }

    return result;
  }
};

#endif // __IMAGE_H__
