#include <++dfb.h>

#include <Insignia.h>
#include <InsigniaTest.h>
#include <InsigniaUtil.h>

#include "DFBfx.h"


class DFBTestBlitting : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestBlitting );

private:
     IDirectFB m_dfb;

public:
     class ConfigBlitting : public Config {
     public:
          ConfigBlitting( DFBSurfacePixelFormat    format,
                          bool                     exclusive,
                          DFBSurfacePixelFormat    image_format,
                          DFBAccelerationMask      blitting_method,
                          unsigned int             drawing_flags,
                          unsigned int             blitting_flags,
                          DFBSurfacePorterDuffRule porter_duff )
          {
               this->format          = format;
               this->exclusive       = exclusive;
               this->image_format    = image_format;
               this->blitting_method = blitting_method;
               this->drawing_flags   = drawing_flags;
               this->blitting_flags  = blitting_flags;
               this->porter_duff     = porter_duff;
          }

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );

               string.PrintF( " | %-8s %s (Image %-8s) %-11s %-8s [Flags %-64s]",
                              dfb_pixelformat_name(format),
                              exclusive ? "not excl." : "EXCLUSIVE",
                              dfb_pixelformat_name(image_format),
                              blitting_method == DFXL_FILLRECTANGLE ? "FillRect" :
                              blitting_method == DFXL_BLIT          ? "Blit"     : "StretchBlit",
                              *ToString<DFBSurfacePorterDuffRule>(porter_duff),
                              blitting_method == DFXL_FILLRECTANGLE ?
                              Insignia::Util::PrintDrawingFlags( (DFBSurfaceDrawingFlags) drawing_flags ).c_str() :
                              Insignia::Util::PrintBlittingFlags( (DFBSurfaceBlittingFlags) blitting_flags ).c_str() );
          }

          DFBSurfacePixelFormat    format;
          bool                     exclusive;
          DFBSurfacePixelFormat    image_format;
          DFBAccelerationMask      blitting_method;
          unsigned int             drawing_flags;
          unsigned int             blitting_flags;
          DFBSurfacePorterDuffRule porter_duff;
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          //DFBSurfacePixelFormat formats[] = { DSPF_ARGB, DSPF_ARGB1555, DSPF_RGB16, DSPF_RGB32 };
          DFBSurfacePixelFormat formats[] = { DSPF_ARGB };

          for (int i=0; i<D_ARRAY_SIZE(formats); i++) {
               bool exclusives[] = { true, false };

               for (int j=0; j<D_ARRAY_SIZE(exclusives); j++) {

                    configs.push_back( new ConfigBlitting( formats[i],
                                                           exclusives[j],
                                                           formats[i],
                                                           DFXL_FILLRECTANGLE,
                                                           DSDRAW_NOFX,
                                                           DSBLIT_NOFX, DSPD_SRC ) );

                    //configs.push_back( new ConfigBlitting( formats[i],
                    //                                       exclusives[j],
                    //                                       formats[i],
                    //                                       DFXL_FILLRECTANGLE,
                    //                                       DSDRAW_BLEND,
                    //                                       DSBLIT_NOFX, DSPD_SRC ) );

                    configs.push_back( new ConfigBlitting( formats[i],
                                                           exclusives[j],
                                                           formats[i],
                                                           DFXL_FILLRECTANGLE,
                                                           DSDRAW_BLEND,
                                                           DSBLIT_NOFX, DSPD_SRC_OVER ) );


                    //DFBSurfacePixelFormat image_formats[] = { DSPF_ARGB, DSPF_ARGB4444, DSPF_A8, DSPF_LUT8 };
                    //DFBSurfacePixelFormat image_formats[] = { formats[i] };
                    //DFBSurfacePixelFormat image_formats[] = { DSPF_ARGB, DSPF_ARGB1555, DSPF_RGB16, DSPF_RGB32 };
                    DFBSurfacePixelFormat image_formats[] = { DSPF_ARGB };

                    for (int k=0; k<D_ARRAY_SIZE(image_formats); k++) {
                         DFBAccelerationMask blitting_methods[] = { DFXL_BLIT, DFXL_STRETCHBLIT };

                         for (int l=0; l<D_ARRAY_SIZE(blitting_methods); l++) {
                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_NOFX, DSPD_SRC ) );

                              //configs.push_back( new ConfigBlitting( formats[i],
                              //                                       exclusives[j],
                              //                                       image_formats[k],
                              //                                       blitting_methods[l],
                              //                                       DSDRAW_NOFX,
                              //                                       DSBLIT_BLEND_ALPHACHANNEL, DSPD_SRC ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_BLEND_ALPHACHANNEL, DSPD_SRC_OVER ) );

                              /*configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_SRC_PREMULTIPLY,    DSPD_SRC ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_SRC_PREMULTIPLY,    DSPD_SRC_OVER ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_BLEND_COLORALPHA   |
                                                                     DSBLIT_SRC_PREMULTCOLOR,   DSPD_SRC ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_BLEND_COLORALPHA   |
                                                                     DSBLIT_SRC_PREMULTCOLOR,   DSPD_SRC_OVER ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_BLEND_COLORALPHA   |
                                                                     DSBLIT_SRC_PREMULTIPLY,    DSPD_SRC ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_BLEND_COLORALPHA   |
                                                                     DSBLIT_SRC_PREMULTIPLY,    DSPD_SRC_OVER ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_COLORIZE,           DSPD_SRC ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_COLORIZE           |
                                                                     DSBLIT_BLEND_ALPHACHANNEL, DSPD_SRC ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_COLORIZE           |
                                                                     DSBLIT_BLEND_ALPHACHANNEL, DSPD_SRC_OVER ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_COLORIZE           |
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_SRC_PREMULTIPLY,    DSPD_SRC ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_COLORIZE           |
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_SRC_PREMULTIPLY,    DSPD_SRC_OVER ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_COLORIZE           |
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_BLEND_COLORALPHA   |
                                                                     DSBLIT_SRC_PREMULTCOLOR,   DSPD_SRC ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_COLORIZE           |
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_BLEND_COLORALPHA   |
                                                                     DSBLIT_SRC_PREMULTCOLOR,   DSPD_SRC_OVER ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_COLORIZE           |
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_BLEND_COLORALPHA   |
                                                                     DSBLIT_SRC_PREMULTIPLY,    DSPD_SRC ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_COLORIZE           |
                                                                     DSBLIT_BLEND_ALPHACHANNEL |
                                                                     DSBLIT_BLEND_COLORALPHA   |
                                                                     DSBLIT_SRC_PREMULTIPLY,    DSPD_SRC_OVER ) );

                              configs.push_back( new ConfigBlitting( formats[i],
                                                                     exclusives[j],
                                                                     image_formats[k],
                                                                     blitting_methods[l],
                                                                     DSDRAW_NOFX,
                                                                     DSBLIT_SRC_COLORKEY,       DSPD_SRC ) );*/
                         }
                    }
               }
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestBlitting";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& config ) {
          const ConfigBlitting& conf = dynamic_cast<const ConfigBlitting&>(config);

          direct_log_printf( NULL, "Format: %-8s  Exclusive: %3s  Image: %-8s  Method: %-11s  PorterDuff: %s\n",
                             dfb_pixelformat_name(conf.format),
                             conf.exclusive ? "YES" : "NO",
                             dfb_pixelformat_name(conf.image_format),
                             conf.blitting_method == DFXL_FILLRECTANGLE ? "FillRect" :
                             conf.blitting_method == DFXL_BLIT          ? "Blit"     : "StretchBlit",
                             conf.porter_duff == DSPD_SRC ? "SRC" : "SRC_OVER" );

          if (conf.blitting_method == DFXL_FILLRECTANGLE)
               Insignia::Util::DumpDrawingFlags( (DFBSurfaceDrawingFlags) conf.drawing_flags );
          else
               Insignia::Util::DumpBlittingFlags( (DFBSurfaceBlittingFlags) conf.blitting_flags );


          m_layer = m_dfb.GetDisplayLayer( DLID_PRIMARY );

          m_layer.SetCooperativeLevel( conf.exclusive ? DLSCL_EXCLUSIVE : DLSCL_ADMINISTRATIVE );

          DFBDisplayLayerConfig lconfig;

          lconfig.flags        = (DFBDisplayLayerConfigFlags)( DLCONF_PIXELFORMAT | DLCONF_BUFFERMODE | DLCONF_SURFACE_CAPS );
          lconfig.pixelformat  = conf.exclusive ? conf.format : DSPF_ARGB;
          lconfig.buffermode   = DLBM_FRONTONLY;
          lconfig.surface_caps = DSCAPS_PREMULTIPLIED;

          m_layer.SetConfiguration( lconfig );
          m_layer.SetBackgroundColor( 0, 0, 0, 0 );
          m_layer.SetBackgroundMode( DLBM_COLOR );

          m_layer_surface = m_layer.GetSurface();

          if (conf.exclusive) {
               m_surface = m_layer_surface;
          }
          else {
               DFBWindowDescription desc;

               desc.flags        = (DFBWindowDescriptionFlags)( DWDESC_POSX | DWDESC_POSY |
                                                                DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS |
                                                                DWDESC_PIXELFORMAT | DWDESC_SURFACE_CAPS );
               desc.posx         = 0;
               desc.posy         = 0;
               desc.width        = 600;
               desc.height       = 400;
               desc.caps         = DWCAPS_NONE;
               desc.pixelformat  = conf.format;
               desc.surface_caps = DSCAPS_PREMULTIPLIED;

               if (DFB_PIXELFORMAT_HAS_ALPHA( desc.pixelformat ))
                    D_FLAGS_SET( desc.caps, DWCAPS_ALPHACHANNEL );

               m_window  = m_layer.CreateWindow( desc );
               m_surface = m_window.GetSurface();

               m_window.SetOpacity( 0xff );
          }

          m_surface.Clear();

          /*
           * Create source surface
           */
          DFBSurfaceDescription sdesc;

          sdesc.flags       = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS );
          sdesc.width       = 100;
          sdesc.height      = 100;
          sdesc.pixelformat = conf.image_format;
          sdesc.caps        = (conf.blitting_flags & DSBLIT_SRC_PREMULTIPLY) ? DSCAPS_NONE : DSCAPS_PREMULTIPLIED;

          m_source = m_dfb.CreateSurface( sdesc );

          /* Load image into source */
          IDirectFBImageProvider provider = m_dfb.CreateImageProvider( *setup.app->Data( "blitting_source.png" ) );

          provider.RenderTo( m_source, NULL );

          m_source.SetSrcColorKey( 115, 161, 69 );

          //dumpSurface( m_source, "source" );

          /*
           * Create reference surface
           */
          sdesc.flags       = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS );
          sdesc.width       = 200;
          sdesc.height      = 200;
          sdesc.pixelformat = conf.format;
          sdesc.caps        = (DFBSurfaceCapabilities)( DSCAPS_SYSTEMONLY | DSCAPS_PREMULTIPLIED );

          m_reference = m_dfb.CreateSurface( sdesc );

          m_reference.Clear();
     }

     virtual Result Run( const Config& config ) {
          Result                result;
          bool                  accel;
          const ConfigBlitting& conf = dynamic_cast<const ConfigBlitting&>(config);

          /* Run test on accelerated (screen) surface */
          accel = runBlitting( conf, m_surface, 100, 50 );

          //dumpSurface( m_surface, "surface" );

          /* Run test on non-accelerated (reference) surface */
          runBlitting( conf, m_reference, 0, 0 );

          /* Copy reference surface to screen surface for user checking */
          m_surface.SetBlittingFlags( DSBLIT_NOFX );
          m_surface.Blit( m_reference, NULL, conf.blitting_method == DFXL_BLIT ? 202 : 302, 50 );

          /* Flip screen surface to show both results */
          m_surface.Flip();

          /*
           * Compare both results at the points listed
           */
          DFBPoint points1[16];
          DFBPoint points2[16];

          int n = 0;

          switch (conf.blitting_method) {
               case DFXL_FILLRECTANGLE:
               case DFXL_BLIT:
                    for (int y=0; y<4; y++) {
                         for (int x=0; x<4; x++) {
                              points1[n] = DFBPoint( 112 + x * 25, 62 + y * 25 );
                              points2[n] = DFBPoint(  12 + x * 25, 12 + y * 25 );

                              n++;
                         }
                    }
                    break;

               case DFXL_STRETCHBLIT:
                    for (int y=0; y<4; y++) {
                         for (int x=0; x<4; x++) {
                              points1[n] = DFBPoint( 125 + x * 50, 75 + y * 50 );
                              points2[n] = DFBPoint(  25 + x * 50, 25 + y * 50 );

                              n++;
                         }
                    }
                    break;

               default:
                    D_BUG( "unexpected blitting function" );
          }


          /* Compare layer surface (for windowed test) and reference */
          Insignia::PixelBuffer layer_buffer    ( m_layer_surface );
          Insignia::PixelBuffer reference_buffer( m_reference );

          switch (conf.blitting_method) {
               case DFXL_FILLRECTANGLE:
               case DFXL_BLIT:
                    layer_buffer.dump( getAndAddExtraFile( "result.png" ), DFBRectangle(100,50,100,100) );
                    reference_buffer.dump( getAndAddExtraFile( "reference.png" ), DFBRectangle(0,0,100,100) );
                    break;

               case DFXL_STRETCHBLIT:
                    layer_buffer.dump( getAndAddExtraFile( "result.png" ), DFBRectangle(100,50,200,200) );
                    reference_buffer.dump( getAndAddExtraFile( "reference.png" ), DFBRectangle(0,0,200,200) );
                    break;

               default:
                    D_BUG( "unexpected blitting function" );
          }

          int fail = Insignia::Util::CompareBuffers( layer_buffer, reference_buffer, points1, points2, 16 );

          if (fail > 0) {
               direct_log_printf( NULL, "TEST FAILED (%d/16 mismatches)!\n", fail );

               result = RESULT_FAILURE;
          }
          else if (accel) {
               direct_log_printf( NULL, "Test succeeded.\n" );

               result = RESULT_SUCCESS;
          }
          else {
               direct_log_printf( NULL, "Test succeeded, but not accelerated.\n" );

               result = RESULT_FALLBACK;
          }

          return result;
     }

     virtual void Cleanup( const Config& config ) {
          m_reference     = NULL;
          m_source        = NULL;
          m_surface       = NULL;
          m_window        = NULL;
          m_layer_surface = NULL;
          m_layer         = NULL;
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }


private:
     bool runBlitting( const ConfigBlitting& conf, IDirectFBSurface surface, int x, int y ) {
          bool                accel = false;
          DFBAccelerationMask mask;

          /*
           * Setup state according to configuration
           */
          surface.SetColor( 161, 69, 115, 128 );
          surface.SetPorterDuff( conf.porter_duff );

          mask = surface.GetAccelerationMask( m_source );

          /* Load image into destination */
          IDirectFBImageProvider provider = m_dfb.CreateImageProvider( *setup.app->Data( "blitting_destination.png" ) );

          DFBRectangle rect( x, y, 100, 100 );

          switch (conf.blitting_method) {
               case DFXL_BLIT:
                    accel = (mask & DFXL_BLIT);

                    provider.RenderTo( surface, &rect );

                    surface.SetBlittingFlags( (DFBSurfaceBlittingFlags) conf.blitting_flags );

                    surface.Blit( m_source, NULL, x, y );
                    break;

               case DFXL_STRETCHBLIT:
                    accel = (mask & DFXL_STRETCHBLIT);

                    rect.w = 200;
                    rect.h = 200;

                    provider.RenderTo( surface, &rect );

                    surface.SetBlittingFlags( (DFBSurfaceBlittingFlags) conf.blitting_flags );

                    surface.StretchBlit( m_source, NULL, &rect );
                    break;

               case DFXL_FILLRECTANGLE:
                    accel = (mask & DFXL_FILLRECTANGLE);

                    provider.RenderTo( surface, &rect );

                    surface.SetDrawingFlags( (DFBSurfaceDrawingFlags) conf.drawing_flags );

                    surface.SetColor( 115, 161, 69, 255 );
                    surface.FillRectangle( x, y, 100, 25 );

                    surface.SetColor( 115 * 170/255, 161 * 170/255, 69 * 170/255, 170 );
                    surface.FillRectangle( x, y + 25, 100, 25 );

                    surface.SetColor( 115 * 85/255, 161 * 85/255, 69 * 85/255, 85 );
                    surface.FillRectangle( x, y + 50, 100, 25 );

                    surface.SetColor( 0, 0, 0, 0 );
                    surface.FillRectangle( x, y + 75, 100, 25 );
                    break;

               default:
                    D_BUG( "unexpected blitting function" );
          }

          return accel;
     }

     IDirectFBDisplayLayer m_layer;
     IDirectFBSurface      m_layer_surface;
     IDirectFBWindow       m_window;
     IDirectFBSurface      m_surface;
     IDirectFBSurface      m_source;
     IDirectFBSurface      m_reference;
};


INSIGNIA_TEST_STATICS( DFBTestBlitting );

