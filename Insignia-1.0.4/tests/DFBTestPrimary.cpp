#include <++dfb.h>

#include <InsigniaTest.h>


class DFBTestPrimary : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestPrimary );

private:
     IDirectFB m_dfb;

public:
     class ConfigPrimary : public Config {
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          configs.push_back( new ConfigPrimary() );
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestPrimary";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual Result Run( const Config& config ) {
          IDirectFBSurface      surface;
          DFBSurfaceDescription desc;

          desc.flags = DSDESC_CAPS;
          desc.caps  = DSCAPS_PRIMARY;

          surface = m_dfb.CreateSurface( desc );

          surface.Clear();

          surface.Flip();

          return RESULT_SUCCESS;
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }
};


INSIGNIA_TEST_STATICS( DFBTestPrimary );

