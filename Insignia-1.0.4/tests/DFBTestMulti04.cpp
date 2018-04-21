#include <++dfb.h>

#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestMulti04 : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestMulti04 );

private:
     IDirectFB           m_dfb;

public:
     struct ConfigMulti04 : public Config {
          ConfigMulti04( unsigned int num_processes ) {
               this->num_processes = num_processes;
          }

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );
          }
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          for (unsigned int i=2; i<10; i++) {
               configs.push_back( new ConfigMulti04( i ));
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestMulti04";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& configuration ) {
     }


     virtual Result Run( const Config& configuration ) {
          const ConfigMulti04& config = dynamic_cast<const ConfigMulti04&>(configuration);

          if (setup.process_index == 0)
               return runMaster( config );

          return runSlave( config );
     }

     virtual void Cleanup( const Config& config ) {
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }


private:
     Result runMaster( const ConfigMulti04& config ) {
          receiveSlavesGo();
          postSlavesGo();

          receiveSlavesExit();

          return RESULT_SUCCESS;
     }


     Result runSlave( const ConfigMulti04& config ) {
          postMasterGo();
          receiveGo();


          IDirectFBDisplayLayer layer   = m_dfb.GetDisplayLayer( DLID_PRIMARY );
          IDirectFBSurface      surface = layer.GetSurface();


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


INSIGNIA_TEST_STATICS( DFBTestMulti04 );

