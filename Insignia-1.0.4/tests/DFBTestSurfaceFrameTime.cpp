#include <++dfb.h>

#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestSurfaceFrameTime : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestSurfaceFrameTime );

private:
     IDirectFB        m_dfb;
     IDirectFBSurface m_surface;

public:
     struct ConfigSurfaceFrameTime : public Config {
          ConfigSurfaceFrameTime( long long interval,
                                  long long max_advance )
               :
               interval( interval ),
               max_advance( max_advance )
          {
          }

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );

               string.PrintF( "  || Interval: %lld  MaxAdvance %lld", interval, max_advance );
          }

          long long interval;
          long long max_advance;
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          long long interval[]    = { 16666, 20000, 25000 };
          long long max_advance[] = { 100000, 200000 };

          for (int i=0; i<D_ARRAY_SIZE(interval); i++) {
               for (int m=0; m<D_ARRAY_SIZE(max_advance); m++) {
                    configs.push_back( new ConfigSurfaceFrameTime( interval[i], max_advance[m] ));
               }
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestSurfaceFrameTime";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& configuration ) {
          const ConfigSurfaceFrameTime& config = dynamic_cast<const ConfigSurfaceFrameTime&>(configuration);

          m_dfb.SetCooperativeLevel( DFSCL_FULLSCREEN );

          DFBSurfaceDescription desc;

          desc.flags  = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS );
          desc.width  = 640;
          desc.height = 480;
          desc.caps   = (DFBSurfaceCapabilities)(DSCAPS_PRIMARY | DSCAPS_DOUBLE);

          m_surface = m_dfb.CreateSurface( desc );


          DFBFrameTimeConfig ftc;

          ftc.flags       = (DFBFrameTimeConfigFlags)(DFTCF_INTERVAL | DFTCF_MAX_ADVANCE);
          ftc.interval    = config.interval;
          ftc.max_advance = config.max_advance;

          m_surface.iface->SetFrameTimeConfig( m_surface.iface, &ftc );
     }


     virtual Result Run( const Config& configuration ) {
          const ConfigSurfaceFrameTime& config = dynamic_cast<const ConfigSurfaceFrameTime&>(configuration);

          int       i;
          long long start  = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );
          long long micros = start;

          for (i=0; i<100 || !stopped(); i++) {
               m_surface.iface->GetFrameTime( m_surface.iface, &micros );

               long long now = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );

               if (micros - now > config.max_advance) {
                    direct_log_printf( NULL, "TEST FAILED (%lld in advance with max at %lld, micros %lld, now %lld, interval %lld)!\n",
                                       micros - now, config.max_advance, micros, now, config.interval );

                    return RESULT_FAILURE;
               }

               m_surface.Clear();

               m_surface.Flip();
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


INSIGNIA_TEST_STATICS( DFBTestSurfaceFrameTime );

