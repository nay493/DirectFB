#include <++dfb.h>

#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestTempSurface : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestTempSurface );

private:
     IDirectFB        m_dfb;
     IDirectFBSurface m_surface;

public:
     struct ConfigTempSurface : public Config {
          ConfigTempSurface( int                    width,
                               int                    height,
                               DFBSurfacePixelFormat  format,
                               DFBSurfaceCapabilities caps,
                               bool                   clear,
                               int                    delay,
                               int                    num )
               :
               width( width ),
               height( height ),
               format( format ),
               caps( caps ),
               clear( clear ),
               delay( delay ),
               num( num )
          {
               num_processes = 2;
          }

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );

               string.PrintF( "  || Size: %4dx%4d  Format: %-8s  Caps: 0x%08x  Clear: %-3s  Delay: %3d ms",
                              width, height, dfb_pixelformat_name(format),
                              caps, clear ? "yes" : "no", delay );
          }

          int                    width;
          int                    height;
          DFBSurfacePixelFormat  format;
          DFBSurfaceCapabilities caps;
          bool                   clear;
          int                    delay;
          int                    num;
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          int                   delays[]  = { 0, 100, 20, 1, 0 };
          int                   num[]     = { 50, 100, 500, 5000 };
          DFBSurfacePixelFormat formats[] = { DSPF_ARGB/*, DSPF_ARGB4444, DSPF_AYUV, DSPF_LUT8*/ };
          DFBDimension_C        sizes[]   = {
               {  10,  10 },
               { 100, 100 },
               { 400, 400 },
               { 900, 900 }
          };

          for (bool clear=false; ; clear=true) {
               for (int d=0; d<D_ARRAY_SIZE(formats); d++) {
                    for (int i=0; i<D_ARRAY_SIZE(formats); i++) {
                         for (int s=0; s<D_ARRAY_SIZE(sizes); s++) {
                              configs.push_back( new ConfigTempSurface( sizes[s].w, sizes[s].h, formats[i], DSCAPS_NONE, clear, delays[d], num[d] ));
                         }
                    }
               }

               if (clear)
                    break;
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestTempSurface";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& configuration ) {
          const ConfigTempSurface& config = dynamic_cast<const ConfigTempSurface&>(configuration);

          DFBSurfaceDescription desc;

          desc.flags       = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS );
          desc.width       = config.width;
          desc.height      = config.height;
          desc.pixelformat = config.format;
          desc.caps        = config.caps;

          m_surface = m_dfb.CreateSurface( desc );
     }


     virtual Result Run( const Config& configuration ) {

          const ConfigTempSurface& config = dynamic_cast<const ConfigTempSurface&>(configuration);

          for (int i=0; !stopped(); i++) {
               DFBSurfaceDescription desc;

               desc.flags       = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS );
               desc.width       = config.width;
               desc.height      = config.height;
               desc.pixelformat = config.format;
               desc.caps        = config.caps;

               IDirectFBSurface surface = m_dfb.CreateSurface( desc );

               //if (config.clear)
                    surface.Clear( i, i, i, i );

               if (config.delay)
                    usleep( config.delay );

               if (i > 0)
                    Insignia::Util::CheckSurface( m_surface, 0, 0, i - 1, i - 1, i - 1, i - 1 );

               m_surface.Blit( surface );

               surface = NULL;
          }

          return RESULT_SUCCESS;
     }

     virtual void Cleanup( const Config& config ) {
          m_surface = NULL;
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }
};


INSIGNIA_TEST_STATICS( DFBTestTempSurface );

