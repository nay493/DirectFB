#include "DFBTestSimple.h"



class ConfigTemplateFont : public Insignia::ConfigTemplate<Insignia::ConfigSimple<ConfigTemplateFont> > {
public:
     int bg_opacity;
     int fg_opacity;

     ConfigTemplateFont()
     {
          addOption( new Insignia::ConfigOptionMinMax<int>( "bg-opacity", bg_opacity, 0, 255, 50 ) );
          addOption( new Insignia::ConfigOptionMinMax<int>( "fg-opacity", fg_opacity, 0, 255, 50 ) );
     }
};


class DFBTestSimpleFont : public DFBTestSimple<ConfigTemplateFont>
{
     INSIGNIA_TEST_CLASS( DFBTestSimpleFont );

public:
     DFBTestSimpleFont() : DFBTestSimple( "Font" ) {}

     virtual Result Run( const Insignia::ConfigSimple<ConfigTemplateFont>& config ) {
          IDirectFBDisplayLayer  layer;

          IDirectFBWindow        window1;
          IDirectFBSurface       window_surface1;
          IDirectFBSurface       window_surface2;

          IDirectFBFont          font;

          DFBDisplayLayerConfig  layer_config;

          static char            testString[] = "Test for transparent text";
          bool                   accel = false;
          DFBFontDescription     desc;

          DFBSurfaceDescription  sdesc;
          DFBWindowDescription   wdesc;
          DFBAccelerationMask    mask;

          layer = dfb.GetDisplayLayer( DLID_PRIMARY );

          layer.SetCooperativeLevel( DLSCL_ADMINISTRATIVE );

          layer.GetConfiguration( &layer_config );

          layer_config.flags = (DFBDisplayLayerConfigFlags)(DLCONF_BUFFERMODE | DLCONF_PIXELFORMAT);
          layer_config.buffermode = DLBM_BACKVIDEO;
          layer_config.pixelformat = DSPF_ABGR;

          layer.SetConfiguration( layer_config );
          layer.EnableCursor ( 0 );

          desc.flags = DFDESC_HEIGHT;
          desc.height = layer_config.height/20 + 1;

          font = dfb.CreateFont( DATADIR"/decker.ttf", desc );



          wdesc.flags = (DFBWindowDescriptionFlags)(DWDESC_CAPS | DWDESC_HEIGHT | DWDESC_WIDTH | DWDESC_POSX | DWDESC_POSY );
          wdesc.caps = DWCAPS_ALPHACHANNEL;
          wdesc.posx = 0;
          wdesc.posy = 0;
          wdesc.width = layer_config.width;
          wdesc.height = layer_config.height;

          window1 = layer.CreateWindow( wdesc );

          window_surface1 = window1.GetSurface();
          window_surface1.SetFont( font );

          window_surface1.SetDrawingFlags( DSDRAW_BLEND);
          window_surface1.Clear( 0, 0, 0, 0);
          window_surface1.Flip();
          window_surface1.Clear( 0, 0, 0, 0);
          window1.SetOpacity( 0xff ) ;

          window_surface1.SetColor( 0xff, 0xff, 0xff, config.bg_opacity); 
          window_surface1.FillRectangle( 0,
                                         0,
                                         layer_config.width,
                                         layer_config.height);

          window_surface1.Flip();
          window_surface1.SetColor( 0xff, 0xff, 0xff, config.fg_opacity); 

          mask = window_surface1.GetAccelerationMask();

          if (mask & DFXL_DRAWSTRING)
               accel = true;

          // Draw text
          window_surface1.DrawString( testString, strlen(testString), 300, 300, DSTF_LEFT);
          window_surface1.Flip();



          sdesc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_HEIGHT | DSDESC_WIDTH | DSDESC_PIXELFORMAT);
          sdesc.caps = DSCAPS_SYSTEMONLY;
          sdesc.width = layer_config.width;
          sdesc.height = layer_config.height;
          sdesc.pixelformat = window_surface1.GetPixelFormat();

          window_surface2 = dfb.CreateSurface( sdesc );
          window_surface2.SetFont( font );

          window_surface2.SetDrawingFlags( DSDRAW_BLEND);
          window_surface2.Clear( 0, 0, 0, 0);
          window_surface2.Flip();
          window_surface2.Clear( 0, 0, 0, 0);

          window_surface2.iface->DisableAcceleration( window_surface2.iface, DFXL_ALL ); 

          window_surface2.SetColor( 0xff, 0xff, 0xff, config.bg_opacity); 
          window_surface2.FillRectangle( 0,
                                         0,
                                         layer_config.width,
                                         layer_config.height);

          window_surface2.Flip();
          window_surface2.SetColor( 0xff, 0xff, 0xff, config.fg_opacity); 
          // Draw text
          window_surface2.DrawString( testString, strlen(testString), 300, 300, DSTF_LEFT);
          window_surface2.Flip();



          Insignia::PixelBuffer result_buffer   ( window_surface1 );
          Insignia::PixelBuffer reference_buffer( window_surface2 );

          int fail = Insignia::Util::CompareBuffers( result_buffer, reference_buffer,
                                                     getAndAddExtraFile( "result.png" ),
                                                     getAndAddExtraFile( "reference.png" ) );

          if (fail > 0) {
               direct_log_printf( NULL, "TEST FAILED (%d/16 mismatches)!\n", fail );

               return RESULT_FAILURE;
          }
          else if (accel) {
               direct_log_printf( NULL, "Test succeeded.\n" );

               return RESULT_SUCCESS;
          }

          direct_log_printf( NULL, "Test succeeded, but not accelerated.\n" );

          return RESULT_FALLBACK;
     }
};


INSIGNIA_TEST_STATICS( DFBTestSimpleFont );

