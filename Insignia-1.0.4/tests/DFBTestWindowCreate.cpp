#include <++dfb.h>

#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestWindowCreate : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestWindowCreate );

private:
     IDirectFB             m_dfb;
     IDirectFBDisplayLayer m_layer;

public:
     struct ConfigWindowCreate : public Config {
          ConfigWindowCreate( int                    width,
                               int                   height,
                               DFBSurfacePixelFormat format,
                               DFBWindowCapabilities caps,
                               bool                  show,
                               int                   delay,
                               int                   num )
               :
               width( width ),
               height( height ),
               format( format ),
               caps( caps ),
               show( show ),
               delay( delay ),
               num( num )
          {
          }

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );

               string.PrintF( "  || Size: %4dx%4d  Format: %-8s  Caps: 0x%08x  Show: %-3s  Delay: %3d ms",
                              width, height, dfb_pixelformat_name(format),
                              caps, show ? "yes" : "no", delay );
          }

          int                   width;
          int                   height;
          DFBSurfacePixelFormat format;
          DFBWindowCapabilities caps;
          bool                  show;
          int                   delay;
          int                   num;
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          int                   delays[]  = { 100000, 1000, 0 };
          int                   num[]     = { 50, 500, 5000 };
          DFBSurfacePixelFormat formats[] = { DSPF_ARGB };
          DFBDimension_C        sizes[]   = {
               {  10,  10 },
               { 400, 400 },
               { 900, 900 }
          };

          for (bool show=false; ; show=true) {
               for (int d=0; d<D_ARRAY_SIZE(delays); d++) {
                    for (int i=0; i<D_ARRAY_SIZE(formats); i++) {
                         for (int s=0; s<D_ARRAY_SIZE(sizes); s++) {
                              configs.push_back( new ConfigWindowCreate( sizes[s].w, sizes[s].h, formats[i], DWCAPS_NONE, show, delays[d], num[d] ));
                              configs.push_back( new ConfigWindowCreate( sizes[s].w, sizes[s].h, formats[i], DWCAPS_DOUBLEBUFFER, show, delays[d], num[d] ));
                         }
                    }
               }

               if (show)
                    break;
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestWindowCreate";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& configuration ) {
          m_layer = m_dfb.GetDisplayLayer( DLID_PRIMARY );
     }


     virtual Result Run( const Config& configuration ) {

          const ConfigWindowCreate& config = dynamic_cast<const ConfigWindowCreate&>(configuration);

          for (int i=0; i<config.num && !stopped(); i++) {
               DFBWindowDescription desc;

               desc.flags       = (DFBWindowDescriptionFlags)( DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_PIXELFORMAT | DWDESC_CAPS );
               desc.width       = config.width;
               desc.height      = config.height;
               desc.pixelformat = config.format;
               desc.caps        = config.caps;

               IDirectFBWindow  window  = m_layer.CreateWindow( desc );
               IDirectFBSurface surface = window.GetSurface();

               if (config.show) {
                    surface.Clear();

                    surface.Flip();

                    window.SetOpacity( 0xff );
               }

               surface = NULL;
               window  = NULL;

               if (config.delay)
                    usleep( config.delay );
          }

          return RESULT_SUCCESS;
     }

     virtual void Cleanup( const Config& config ) {
          m_layer = NULL;
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }
};


INSIGNIA_TEST_STATICS( DFBTestWindowCreate );

