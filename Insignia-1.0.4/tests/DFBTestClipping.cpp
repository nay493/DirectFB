#include <++dfb.h>

#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestClipping : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestClipping );

private:
     IDirectFB m_dfb;

     IDirectFBSurface      surface_from;
     IDirectFBSurface      surface_to;
     IDirectFBSurface      surface_reference;

     DFBRegion surface_area;

public:
     struct ConfigClipping : public Config {
          ConfigClipping( DFBRegion &r,
                          DFBSurfacePixelFormat pf_from,
                          DFBSurfacePixelFormat pf_to,
                          DFBSurfaceCapabilities caps_from,
                          DFBSurfaceCapabilities caps_to ) {

               DFB_ADD_SURFACE_CAPS(caps_from,DSCAPS_SHARED);
               DFB_ADD_SURFACE_CAPS(caps_to,DSCAPS_SHARED);

               clip            = r;
               this->pf_from   = pf_from;
               this->pf_to     = pf_to;
               this->caps_from = caps_from;
               this->caps_to   = caps_to;
          }

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );

               string.PrintF( " | %-8s (Caps %-30s) to %-8s (Caps %-30s) [Clip %4d,%4d-%4dx%4d]",
                              dfb_pixelformat_name(pf_from),
                              *ToString<DFBSurfaceCapabilities>(caps_from),
                              dfb_pixelformat_name(pf_to),
                              *ToString<DFBSurfaceCapabilities>(caps_to),
                              DFB_RECTANGLE_VALS_FROM_REGION(&clip) );
          }

          DFBSurfacePixelFormat pf_from;
          DFBSurfacePixelFormat pf_to;

          DFBSurfaceCapabilities caps_from;
          DFBSurfaceCapabilities caps_to;

          DFBRegion clip;
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          DFBSurfacePixelFormat pf_from[] = { DSPF_ARGB, DSPF_RGB16 };
          DFBSurfacePixelFormat pf_to[]   = { DSPF_ARGB, DSPF_RGB16 };
          DFBRegion clips[] = { DFBRegion(0,0,96,96), DFBRegion(21,31,96,96), DFBRegion(0,0,32,32),
                                DFBRegion(0,0,33,33), DFBRegion(11,41,57,55) };

          for (int i=0; i<D_ARRAY_SIZE(pf_from); i++) {
               for (int j=0; j<D_ARRAY_SIZE(pf_to); j++) {
                    for (int k=0; k<D_ARRAY_SIZE(clips); k++) {
                         configs.push_back( new ConfigClipping( clips[k], pf_from[i], pf_to[j], DSCAPS_NONE, DSCAPS_NONE ));
                    }
               }
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestClipping";
     }

     virtual void Initialize( unsigned int process_index ) {
          DFBRegion r( 0, 0, 96, 96 ); /* no multiple of 3 or 4 */

          m_dfb        = DirectFB::Create();
          surface_area = r;
     }

     virtual void Prepare( const Config& configuration ) {
          const ConfigClipping& config = dynamic_cast<const ConfigClipping&>(configuration);

          DFBSurfaceDescription desc;

          desc.flags  = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH |
                         DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS );
          desc.width  = surface_area.x2 - surface_area.x1 + 1;
          desc.height = surface_area.y2 - surface_area.y1 + 1;

          desc.pixelformat  = config.pf_from;
          desc.caps         = config.caps_from;
          surface_from      = m_dfb.CreateSurface( desc );

          desc.pixelformat  = config.pf_to;
          desc.caps         = config.caps_to;
          surface_to        = m_dfb.CreateSurface( desc );
          surface_reference = m_dfb.CreateSurface( desc );
     }


     virtual Result Run( const Config& configuration ) {

          const ConfigClipping& config = dynamic_cast<const ConfigClipping&>(configuration);

          DFBColor   red = {255,255,0,0};
          DFBColor clear = {0,0,0,0};
          int       fail1 = 0;
          int       fail1b = 0;
          int       fail2 = 0;
          int       fail2b = 0;
          int       fail = 0;

          DFBRectangle from, to, clip;
          from.x = from.y = 25;
          from.w = from.h = 37;
          to.x = to.y = 25;
          to.w = to.h = 50;

          // --------------------------------------------------------------
          // first, we test clipping by using a simple all red test picture

          surface_from.Clear(red);

          surface_to.Clear();
          surface_to.SetClip(&config.clip);
          surface_to.StretchBlit( surface_from, &from, &to );

          /* reference picture, using manual clipping */
          clip.x = MAX(config.clip.x1,to.x);
          clip.y = MAX(config.clip.y1,to.y);
          clip.w = MIN(config.clip.x2,to.x+to.w-1) - clip.x + 1;
          clip.h = MIN(config.clip.y2,to.y+to.h-1) - clip.y + 1;
          surface_reference.Clear();
          if ( (clip.w > 0) && (clip.h > 0) ) {
               surface_reference.SetClip();
               surface_reference.SetColor(red);
               surface_reference.FillRectangle(clip);
          }

          fail1 += Insignia::Util::CompareSurfaces( surface_to, surface_reference,
                                                   getAndAddExtraFile( "test1_result.png" ),
                                                   getAndAddExtraFile( "test1_reference.png" ) );
          if (fail1) {
               direct_log_printf( NULL, "TEST 1 FAILED (%d mismatches)!\n", fail1 );

               usleep( 50000 );

               fail1b += Insignia::Util::CompareSurfaces( surface_to, surface_reference,
                                                        getAndAddExtraFile( "test1b_result.png" ),
                                                        getAndAddExtraFile( "test1b_reference.png" ) );
               if (fail1b)
                    direct_log_printf( NULL, "TEST 1b FAILED (%d mismatches)!\n", fail1b );
          }

          // --------------------------------------------------------------
          // secondly, we test distortion by using a "proper" picture as source

          surface_from.Clear();
          for(int i=0;i<surface_area.x2;i+=3)
               for(int j=0;j<surface_area.y2;j+=3) {
                    DFBColor c = { 255, (u8) (i*4), (u8) (j*4), 255 };
                    surface_from.SetColor(c);
                    surface_from.FillRectangle(i,j,2,2);
                    surface_from.FillRectangle(i+1,j+1,2,2);
               }

          for (int i=0; i<2; i++) {
               IDirectFBSurface &surface = (i == 0) ? surface_to : surface_reference;

               DFBRegion blacken;
               surface.Clear();
               surface.SetClip();
               surface.StretchBlit( surface_from, &from, &to );
               surface.SetColor(clear);

               if (config.clip.x1 > 0) {
                    blacken = surface_area;
                    blacken.x2 = config.clip.x1 - 1;
                    surface.FillRectangle(blacken);
               }
               if (config.clip.y1 > 0) {
                    blacken = surface_area;
                    blacken.y2 = config.clip.y1 - 1;
                    surface.FillRectangle(blacken);
               }
               if (config.clip.x2 < surface_area.x2) {
                    blacken = surface_area;
                    blacken.x1 = config.clip.x2 + 1;
                    surface.FillRectangle(blacken);
               }
               if (config.clip.y2 < surface_area.y2) {
                    blacken = surface_area;
                    blacken.y1 = config.clip.y2 + 1;
                    surface.FillRectangle(blacken);
               }
          }

          fail2 += Insignia::Util::CompareSurfaces( surface_to, surface_reference,
                                                   getAndAddExtraFile( "test2_result.png" ),
                                                   getAndAddExtraFile( "test2_reference.png" ) );
          if (fail2 > 0) {
               direct_log_printf( NULL, "TEST 2 FAILED (%d mismatches total)!\n", fail2 );

               usleep( 50000 );

               fail2b += Insignia::Util::CompareSurfaces( surface_to, surface_reference,
                                                        getAndAddExtraFile( "test2b_result.png" ),
                                                        getAndAddExtraFile( "test2b_reference.png" ) );
               if (fail2b)
                    direct_log_printf( NULL, "TEST 2b FAILED (%d mismatches)!\n", fail2b );
          }

          fail = fail1 + fail1b + fail2 + fail2b;

          return (fail) ? RESULT_FAILURE : RESULT_SUCCESS;
     }

     virtual void Cleanup( const Config& config ) {
          surface_from       = NULL;
          surface_to         = NULL;
          surface_reference  = NULL;
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }
};


INSIGNIA_TEST_STATICS( DFBTestClipping );

