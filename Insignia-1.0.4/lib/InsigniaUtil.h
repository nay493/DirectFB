#ifndef __INSIGNIA__UTIL_H__
#define __INSIGNIA__UTIL_H__

#include "common.h"

#define PRINT_US_MS(__us)     ((__us) / 1000LL), ((__us) % 1000LL)


namespace Insignia {


class PixelBuffer
{
public:
     DFBDimension             size;
     DFBSurfacePixelFormat    format;
     void                    *data;
     int                      pitch;
     bool                     free_data;

     PixelBuffer( const DFBDimension    &size,
                  DFBSurfacePixelFormat  format,
                  void                  *data,
                  int                    pitch )
          :
          size( size ),
          format( format ),
          data( data ),
          pitch( pitch ),
          free_data( false )
     {
     }

     PixelBuffer( IDirectFBSurface& surface )
          :
          format( DSPF_UNKNOWN ),
          data( NULL ),
          pitch( 0 ),
          free_data( true )
     {
          surface.GetSize( &size.w, &size.h );

          format = surface.GetPixelFormat();


          void *ptr;

          surface.Lock( DSLF_READ, &ptr, &pitch );

          data = D_MALLOC( pitch * size.h );
          if (!data)
               D_OOM();
          else
               direct_memcpy( data, ptr, pitch * size.h );

          surface.Unlock();
     }

     ~PixelBuffer()
     {
          if (data && free_data)
               D_FREE( data );
     }

     static const DFBRectangle noRect;

     void dump( const Direct::String &filename,
                const DFBRectangle   &rect = noRect );
};


class Util {

public:
     static const char *BlendFunctionToString   ( DFBSurfaceBlendFunction func );
     static const char *AccelerationMaskToString( DFBAccelerationMask     func );

     static void     DumpBlittingFlags( DFBSurfaceBlittingFlags flags );
     static void     DumpDrawingFlags( DFBSurfaceDrawingFlags flags );
     static void     DumpBlendFunctions( DFBSurfaceBlendFunction src, DFBSurfaceBlendFunction dst );

     static std::string   PrintDrawingFlags( DFBSurfaceDrawingFlags flags );
     static std::string   PrintBlittingFlags( DFBSurfaceBlittingFlags flags );

     static int CompareSurfaces( IDirectFBSurface&  surface,
                                 IDirectFBSurface&  reference,
                                 const DFBPoint    *points1,
                                 const DFBPoint    *points2,
                                 int                num,
                                 const std::string &file_surface = std::string(),
                                 const std::string &file_reference = std::string() );

     static int CompareBuffers( PixelBuffer&       surface,
                                PixelBuffer&       reference,
                                const DFBPoint    *points1,
                                const DFBPoint    *points2,
                                int                num,
                                const std::string &file_surface = std::string(),
                                const std::string &file_reference = std::string() );

     static int CompareBuffers( PixelBuffer&       surface,
                                PixelBuffer&       reference,
                                const std::string &file_surface = std::string(),
                                const std::string &file_reference = std::string() );

     // compare the whole surface - only works if pixelformat is SAME
     static int CompareSurfaces( IDirectFBSurface&  surface,
                                 IDirectFBSurface&  reference,
                                 const std::string &file_surface = std::string(),
                                 const std::string &file_reference = std::string() );

     static int CheckSurface( IDirectFBSurface&  surface,
                              const DFBPoint    *points,
                              const DFBColor    *colors,
                              int                num );

     static int CheckSurface( IDirectFBSurface&  surface,
                              int                x,
                              int                y,
                              int                a,
                              int                r,
                              int                g,
                              int                b );

     static DFBSurfaceCapabilities BufferCaps( unsigned int num_buffers );
};



}

#endif

