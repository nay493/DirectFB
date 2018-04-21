#include <++dfb.h>

#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestMulti01 : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestMulti01 );

private:
     IDirectFB           m_dfb;

     IDirectFBSurface    surface;

public:
     struct ConfigMulti01 : public Config {
          ConfigMulti01( unsigned int num_processes ) {
               this->num_processes = num_processes;
          }

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );
          }
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          for (unsigned int i=2; i<10; i++) {
               configs.push_back( new ConfigMulti01( i ));
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestMulti01";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& configuration ) {
          if (setup.process_index > 0) {
               DFBSurfaceDescription desc;

               desc.flags  = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH  |
                                                           DSDESC_HEIGHT |
                                                           DSDESC_CAPS );
               desc.width  = 400;
               desc.height = 400;
               desc.caps   = DSCAPS_SHARED;

               surface     = m_dfb.CreateSurface( desc );
          }
     }


     virtual Result Run( const Config& configuration ) {
          const ConfigMulti01& config = dynamic_cast<const ConfigMulti01&>(configuration);

          if (setup.process_index == 0)
               return runMaster( config );

          return runSlave( config );
     }

     virtual void Cleanup( const Config& config ) {
          surface = NULL;
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }


private:
     Result runMaster( const ConfigMulti01& config ) {
          receiveSlavesGo();
          postSlavesGo();

          receiveSlavesExit();

          return RESULT_SUCCESS;
     }


     Result runSlave( const ConfigMulti01& config ) {
          postMasterGo();
          receiveGo();


          for (int i=0; !stopped(); i++) {
               surface.Clear( setup.process_index * 0x1234, setup.process_index * 0x2345, setup.process_index * 0x3456, setup.process_index * 0x4567 );

               surface.SetColor( 0, 0, 0, 0 );
               surface.FillRectangle( i % (400-10), 10, 10, 10 );

               surface.Flip();
          }


          postMasterExit();

          return RESULT_SUCCESS;
     }
};


INSIGNIA_TEST_STATICS( DFBTestMulti01 );

