#include <++dfb.h>

#include "InsigniaException.h"
#include "InsigniaTest.h"
#include "InsigniaUtil.h"

extern "C" {
#include <png.h>
}

/**********************************************************************************************************************/

template<>
ToString<Insignia::Test::Result>::ToString( const Insignia::Test::Result &v )
{
     switch (v) {
          case Insignia::Test::RESULT_SUCCESS:
               PrintF( "Success" );
               break;

          case Insignia::Test::RESULT_FALLBACK:
               PrintF( "Fallback" );
               break;

          case Insignia::Test::RESULT_FAILURE:
               PrintF( "FAILURE" );
               break;

          case Insignia::Test::RESULT_SIGNAL:
               PrintF( "SIGNAL" );
               break;

          case Insignia::Test::RESULT_SEGFAULT:
               PrintF( "SEGFAULT" );
               break;

          case Insignia::Test::RESULT_EXCEPTION:
               PrintF( "EXCEPTION" );
               break;

          case Insignia::Test::RESULT_SHUTDOWN:
               PrintF( "SHUTDOWN" );
               break;

          case Insignia::Test::RESULT_PREPARE:
               PrintF( "PREPARE" );
               break;

          case Insignia::Test::RESULT_INITIALISE:
               PrintF( "INITIALISE" );
               break;

          default:
               PrintF( "UNKNOWN RESULT 0x%x", v );
               break;
     }
}

template<>
ToString<Insignia::Test::Status>::ToString( const Insignia::Test::Status &v )
{
     PrintF( "%s", *ToString<Insignia::Test::Result>(v.result) );

     if (v.signal)
          PrintF( "-%d", v.signal );

     if (v.message[0])
          PrintF( "-[%s]", v.message );
}

/**********************************************************************************************************************/

namespace Insignia {

static const DirectFBPixelFormatNames(format_names);
static const DirectFBSurfaceBlittingFlagsNames(blittingflags_names);
static const DirectFBSurfaceDrawingFlagsNames(drawingflags_names);
static const DirectFBAccelerationMaskNames(accelerationmask_names);
static const DirectFBSurfaceBlendFunctionNames(blendfuncs_names);


const char *
Util::BlendFunctionToString( DFBSurfaceBlendFunction func )
{
     int i;

     for (i=0; i<D_ARRAY_SIZE(blendfuncs_names); i++) {
          if (blendfuncs_names[i].function == func)
               return blendfuncs_names[i].name;
     }

     return "<unknown>";
}

const char *
Util::AccelerationMaskToString( DFBAccelerationMask func )
{
     int i;

     for (i=0; i<D_ARRAY_SIZE(accelerationmask_names); i++) {
          if (accelerationmask_names[i].mask == func)
               return accelerationmask_names[i].name;
     }

     return "<unknown>";
}

void
Util::DumpBlittingFlags( DFBSurfaceBlittingFlags flags )
{
     /* Show blitting flags being used. */
     direct_log_printf( NULL, "  blittingflags: %s\n", PrintBlittingFlags(flags).c_str() );
}

void
Util::DumpDrawingFlags( DFBSurfaceDrawingFlags flags )
{
     /* Show blitting flags being used. */
     direct_log_printf( NULL, "  drawingflags: %s\n", PrintDrawingFlags(flags).c_str() );
}

void
Util::DumpBlendFunctions( DFBSurfaceBlendFunction src, DFBSurfaceBlendFunction dst )
{
     direct_log_printf( NULL, "  src_blend:  %s\n", BlendFunctionToString( src ) );
     direct_log_printf( NULL, "  dst_blend:  %s\n", BlendFunctionToString( dst ) );
}

std::string
Util::PrintDrawingFlags( DFBSurfaceDrawingFlags flags )
{
     int         i;
     bool        any = false;
     std::string result;

     /* Print drawing flags being used. */
     for (i=0; i<D_ARRAY_SIZE(drawingflags_names); i++) {
          if (D_FLAGS_IS_SET( flags, drawingflags_names[i].flag )) {
               if (any)
                    result += " ";

               result += drawingflags_names[i].name;

               any = true;
          }
     }

     if (!any)
          result = "NOFX";

     return result;
}

std::string
Util::PrintBlittingFlags( DFBSurfaceBlittingFlags flags )
{
     int         i;
     bool        any = false;
     std::string result;

     /* Print blitting flags being used. */
     for (i=0; i<D_ARRAY_SIZE(blittingflags_names); i++) {
          if (D_FLAGS_IS_SET( flags, blittingflags_names[i].flag )) {
               if (any)
                    result += " ";

               result += blittingflags_names[i].name;

               any = true;
          }
     }

     if (!any)
          result = "NOFX";

     return result;
}

/******************************************************************************/

int
Util::CompareSurfaces( IDirectFBSurface&  surface,
                       IDirectFBSurface&  reference,
                       const DFBPoint    *points1,
                       const DFBPoint    *points2,
                       int                num,
                       const std::string &file_surface,
                       const std::string &file_reference )
{
     Insignia::PixelBuffer buffer_surface  ( surface );
     Insignia::PixelBuffer buffer_reference( reference );

     return CompareBuffers( buffer_surface, buffer_reference, points1, points2, num, file_surface, file_reference );
}

int
Util::CompareBuffers( PixelBuffer&       surface,
                      PixelBuffer&       reference,
                      const DFBPoint    *points1,
                      const DFBPoint    *points2,
                      int                num,
                      const std::string &file_surface,
                      const std::string &file_reference )
{
     D_ASSERT( points1 != NULL );
     D_ASSERT( points2 != NULL );
     D_ASSERT( num > 0 );


     DFBSurfacePixelFormat f1 = surface.format;
     DFBSurfacePixelFormat f2 = reference.format;

     void *d1 = surface.data,  *d2 = reference.data;
     int   p1 = surface.pitch,  p2 = reference.pitch;

     if (!file_surface.empty())
          surface.dump( file_surface.c_str() );

     if (!file_reference.empty())
          reference.dump( file_reference.c_str() );

     int fail = 0;

     for (int i=0; i<num; i++) {
          DFBColor c1, c2;

//          direct_log_printf(NULL,"%d,%d\n",points[i].x,points[i].y);

          // FIXME: support planar, three-byte and palette formats
          switch (DFB_BYTES_PER_PIXEL(f1)) {
               case 4: {
                    u32 *ptr = (u32*) ((char*) d1 + points1[i].x * 4 + points1[i].y * p1);

                    dfb_pixel_to_color( f1, *ptr, &c1 );
                    break;
               }

               case 2: {
                    u16 *ptr = (u16*) ((char*) d1 + points1[i].x * 2 + points1[i].y * p1);

                    dfb_pixel_to_color( f1, *ptr, &c1 );
                    break;
               }

               default:
                    D_BUG( "unsupported 1st format '%s'", dfb_pixelformat_name(f1) );
                    D_UNIMPLEMENTED();
                    return num;
          }

          // FIXME: support planar, three-byte and palette formats
          switch (DFB_BYTES_PER_PIXEL(f2)) {
               case 4: {
                    u32 *ptr = (u32*) ((char*) d2 + points2[i].x * 4 + points2[i].y * p2);

                    dfb_pixel_to_color( f2, *ptr, &c2 );
                    break;
               }

               case 2: {
                    u16 *ptr = (u16*) ((char*) d2 + points2[i].x * 2 + points2[i].y * p2);

                    dfb_pixel_to_color( f2, *ptr, &c2 );
                    break;
               }

               default:
                    D_BUG( "unsupported 2nd format '%s'", dfb_pixelformat_name(f2) );
                    D_UNIMPLEMENTED();
                    return num;
          }

          if (ABS((int) c1.a - c2.a) > TOLERANCE ||
              ABS((int) c1.r - c2.r) > TOLERANCE ||
              ABS((int) c1.g - c2.g) > TOLERANCE ||
              ABS((int) c1.b - c2.b) > TOLERANCE)
          {
               fail++;

               direct_log_printf( NULL, "MISMATCH at %4d,%4d (%02x %02x %02x %02x != %02x %02x %02x %02x)\n",
                                  points1[i].x, points1[i].y, c1.a, c1.r, c1.g, c1.b, c2.a, c2.r, c2.g, c2.b );
          }
     }

     return fail;
}

int
Util::CheckSurface( IDirectFBSurface&  surface,
                    const DFBPoint    *points,
                    const DFBColor    *colors,
                    int                num )
{
//     D_ASSERT( surface );
//     D_ASSERT( reference );
     D_ASSERT( points != NULL );
     D_ASSERT( colors != NULL );
     D_ASSERT( num > 0 );

//     surface.Dump( ".", "compare_surface" );
//     reference.Dump( ".", "compare_reference" );


     DFBSurfacePixelFormat f1 = surface.GetPixelFormat();

     void *d1;
     int   p1;

     surface.Lock( DSLF_READ, &d1, &p1 );

     int fail = 0;

     for (int i=0; i<num; i++) {
          DFBColor c1, c2;

//          direct_log_printf(NULL,"%d,%d\n",points[i].x,points[i].y);

          // FIXME: support planar, three-byte and palette formats
          switch (DFB_BYTES_PER_PIXEL(f1)) {
               case 4: {
                    u32 *ptr = (u32*) ((char*) d1 + points[i].x * 4 + points[i].y * p1);

                    dfb_pixel_to_color( f1, *ptr, &c1 );
                    break;
               }

               case 2: {
                    u16 *ptr = (u16*) ((char*) d1 + points[i].x * 2 + points[i].y * p1);

                    dfb_pixel_to_color( f1, *ptr, &c1 );
                    break;
               }

               default:
                    D_UNIMPLEMENTED();
                    return num;
          }

          c2 = colors[i];

          if (ABS((int) c1.a - c2.a) > TOLERANCE ||
              ABS((int) c1.r - c2.r) > TOLERANCE ||
              ABS((int) c1.g - c2.g) > TOLERANCE ||
              ABS((int) c1.b - c2.b) > TOLERANCE)
          {
               fail++;

               direct_log_printf( NULL, "MISMATCH at %4d,%4d (%02x %02x %02x %02x != %02x %02x %02x %02x)\n",
                                  points[i].x, points[i].y, c1.a, c1.r, c1.g, c1.b, c2.a, c2.r, c2.g, c2.b );
          }
     }

     surface.Unlock();

     return fail;
}

int
Util::CheckSurface( IDirectFBSurface&  surface,
                    int                x,
                    int                y,
                    int                a,
                    int                r,
                    int                g,
                    int                b )
{
     DFBPoint point( x, y );
     DFBColor color = { (u8) a, (u8) r, (u8) g, (u8) b };

     return CheckSurface( surface, &point, &color, 1 );
}

// compare two surfaces on byte level, so the pixel formats must be equal
//
// This function does not support the following:
//   - empty bytes, as in RGB32
//   - multiple planes
//   - sub-byte formats, as A1
//   - tolerance for non-byte-aligned-formats

int
Util::CompareBuffers( Insignia::PixelBuffer &buffer_surface,
                      Insignia::PixelBuffer &buffer_reference,
                      const std::string     &file_surface,
                      const std::string     &file_reference )
{
     int element_size = 0;

     DFBDimension size = buffer_surface.size;

     if ( (buffer_surface.size.w!=buffer_reference.size.w) || (buffer_surface.size.h!=buffer_reference.size.h) )
          return 1;

     if (!file_surface.empty())
          buffer_surface.dump( file_surface.c_str() );

     if (!file_reference.empty())
          buffer_reference.dump( file_reference.c_str() );

     DFBSurfacePixelFormat format = buffer_surface.format;
     DFBSurfacePixelFormat f_temp = buffer_reference.format;

     if (format != f_temp)
          return 1;

     switch (format) {
          case DSPF_RGB24:
          case DSPF_ARGB:
          case DSPF_ABGR:
          case DSPF_A8:
          case DSPF_YUY2:
          case DSPF_UYVY:
          case DSPF_LUT8:
          case DSPF_AiRGB:
          case DSPF_AYUV:
          case DSPF_AVYU:
          case DSPF_VYU:
               element_size = 8;
               break;
          case DSPF_ARGB4444:
          case DSPF_RGBA4444:
          case DSPF_ALUT44:
               element_size = 4;
               break;

          /* following formats have no equal-sized ARGBs or not byte aligned */
          case DSPF_ARGB1555:
          case DSPF_RGB16:
          case DSPF_RGB332:
          case DSPF_ARGB2554:
          case DSPF_ARGB6666:
          case DSPF_RGB444:
          case DSPF_RGBA5551:
          case DSPF_ARGB8565:
               break;
          default:
               return 1;
     }

     void          *v1 = buffer_surface.data, *v2 = buffer_reference.data;
     unsigned char *d1 ,*d2;
     int            p1 = buffer_surface.pitch,  p2 = buffer_reference.pitch;

     d1 = (unsigned char*)v1;
     d2 = (unsigned char*)v2;

     int bpl = DFB_BYTES_PER_LINE(format,size.w);

     int fail = 0;
     int res;

     for (int h=0; h<size.h; h++) {

          res = memcmp( d1, d2, bpl );

          /* try using TOLERANCE */
          if (res && element_size ) {
               int w, diff, diff2;
               res = 0;

               if (element_size == 8)
                    for (w=0; w<bpl; w++) {
                         diff = d1[w] - d2[w];
                         if ( (diff < -TOLERANCE) || (diff > TOLERANCE) ) {
                              res = 1;
                              break;
                         }
                    }
               else
                    for (w=0; w<bpl; w++) {
                         diff  = (d1[w]&0xf) - (d2[w]&0xf);
                         diff2 =  (d1[w]>>4) - (d2[w]>>4);
                         if (    (diff  < -TOLERANCE)
                              || (diff  > TOLERANCE)
                              || (diff2 < -TOLERANCE)
                              || (diff2 > TOLERANCE) ) {
                              res = 1;
                              break;
                         }
                    }

          }

          if (res) {
               fail++;
               direct_log_printf( NULL, "MISMATCH at line %d\n", h );
          }

          d1 += p1;
          d2 += p2;
     }

     return fail;
}

int
Util::CompareSurfaces( IDirectFBSurface&  surface,
                       IDirectFBSurface&  reference,
                       const std::string &file_surface,
                       const std::string &file_reference )
{
     Insignia::PixelBuffer result_buffer   ( surface );
     Insignia::PixelBuffer reference_buffer( reference );

     return CompareBuffers( result_buffer, reference_buffer, file_surface, file_reference );
}

DFBSurfaceCapabilities
Util::BufferCaps( unsigned int num_buffers )
{
     switch (num_buffers) {
          case 1:
               break;

          case 2:
               return DSCAPS_DOUBLE;

          case 3:
               return DSCAPS_TRIPLE;

          default:
               D_BUG( "invalid num buffers %u", num_buffers );
     }

     return DSCAPS_NONE;
}



static void
stdio_write_func (png_structp png, png_bytep data, png_size_t size)
{
    FILE *fp;

    fp = (FILE*) png_get_io_ptr (png);
    while (size) {
	size_t ret = fwrite (data, 1, size, fp);
	size -= ret;
	data += ret;
	if (size && ferror (fp)) {
	    DFBResult *error = (DFBResult *)png_get_error_ptr (png);
	    if (*error == DFB_OK)
		*error = DFB_IO;
	    png_error (png, NULL);
	}
    }
}

const DFBRectangle PixelBuffer::noRect;

void
PixelBuffer::dump( const Direct::String &filename,
                   const DFBRectangle   &rect )
{
     DirectResult  result = DR_OK;
     void         *argb   = data;
     int           apitch = pitch;

     FILE         *fp;
     png_struct   *png;
     png_info     *info;

     DFBRectangle  r( size );

     fp = fopen( filename.buffer(), "wb" );
     if (fp == NULL)
          throw new Exception( Direct::String::F( "PixelBuffer::dump: Could not open '%s' for writing!", filename.buffer() ) );

     if (&rect != &noRect) {
          if (!dfb_rectangle_intersect( &r, &rect )) {
               throw new Exception( Direct::String::F( 
                    "PixelBuffer::dump: No intersection ["DFB_RECT_FORMAT" vs "DFB_RECT_FORMAT"] writing to '%s'!",
                    DFB_RECTANGLE_VALS(&rect), DFB_RECTANGLE_VALS(&r), filename.buffer() ) );
          }
     }

     if (format != DSPF_ARGB) {
          apitch = r.w * 4;

          argb = D_MALLOC( apitch * r.h );
          if (!argb) {
               result = D_OOM();
               goto out;
          }

          dfb_convert_to_argb( format, (png_bytep) data + r.x * 4 + r.y * pitch,
                               pitch, NULL, 0, NULL, 0, size.h, (u32*) argb, apitch, r.w, r.h );

          r.x = r.y = 0;
     }

     png  = png_create_write_struct(PNG_LIBPNG_VER_STRING, &result, 0, 0);
     info = png_create_info_struct(png);

     if (!png || !info || setjmp(png_jmpbuf(png))) {
          png_destroy_write_struct(png ? &png : 0, info ? &info : 0);
          goto out;
     }

     png_set_compression_level(png, 3);
     png_set_filter(png, PNG_FILTER_TYPE_BASE, PNG_FILTER_SUB);

     png_set_write_fn(png, fp, stdio_write_func, 0);
     png_set_bgr(png);
     png_set_IHDR(png, info, r.w, r.h,
                  8, PNG_COLOR_TYPE_RGB_ALPHA, 0, 0, 0);
     png_write_info(png, info);

     for (int y = 0; y < r.h; ++y)
          png_write_row( png, (png_bytep) argb + r.x * 4 + (r.y + y) * apitch );

     png_write_end(png, info);
     png_destroy_write_struct(&png, &info);


out:
     fclose( fp );

     if (argb && argb != data)
          D_FREE( argb );

     if (result) {
          unlink( filename.buffer() );

          throw new Exception( Direct::String::F( "PixelBuffer::dump: Could not write PNG data to '%s' (%s)!", filename.buffer(),
                                                  DirectResultString(result) ) );
     }
}


}

