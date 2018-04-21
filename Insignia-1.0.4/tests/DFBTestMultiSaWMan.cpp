#include <++dfb.h>

#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestMultiSaWMan : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestMultiSaWMan );

private:
     IDirectFB           m_dfb;

     IDirectFBDisplayLayer layer;
     IDirectFBSurface      surface;
     ISaWMan              *sawman;

public:
     struct ConfigMultiSaWMan : public Config {
          ConfigMultiSaWMan( unsigned int num_processes ) {
               this->num_processes = num_processes;
          }

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );
          }
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          for (unsigned int i=2; i<32; i++) {
               configs.push_back( new ConfigMultiSaWMan( i ));
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestMultiSaWMan";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& configuration ) {
          layer = m_dfb.GetDisplayLayer( DLID_PRIMARY );
     }


     virtual Result Run( const Config& configuration ) {
          const ConfigMultiSaWMan& config = dynamic_cast<const ConfigMultiSaWMan&>(configuration);

          if (setup.process_index == 0)
               return runMaster( config );

          return runSlave( config );
     }

     virtual void Cleanup( const Config& config ) {
          surface = NULL;
          layer = NULL;
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }


private:
     Result runMaster( const ConfigMultiSaWMan& config ) {
          receiveSlavesGo();

          postSlavesGo();


          MultiSwitchPacket packet;

          Post( 1, packet );
          Post( 1, packet );


          receiveSlavesExit();

          return RESULT_SUCCESS;
     }


     Result runSlave( const ConfigMultiSaWMan& config ) {
          postMasterGo();

          receiveGo();


          layer.SetCooperativeLevel( DLSCL_EXCLUSIVE );
#if 0
          DFBDisplayLayerConfig config; //  = config.getLayerConfig(slave)

          desc.flags  = (DFBDisplayLayerConfigFlags)( DLCONF_WIDTH  |
                                                      DLCONF_HEIGHT |
                                                      DLCONF_BUFFERMODE );
          desc.width  = 1280;
          desc.height = 720;
          desc.caps   = DSCAPS_SHARED;
#endif
          surface     = layer.GetSurface();

          for (int i=0; !stopped(); i++) {
          //     surface.Clear( setup.process_index * 0x1234, setup.process_index * 0x2345, setup.process_index * 0x3456, setup.process_index * 0x4567 );

               surface.SetColor( 0, 0, 0, 0 );
               surface.FillRectangle( i % (400-10), 10, 10, 10 );

               surface.Flip();


               if (GotPacket( MultiSwitchPacket::TYPE_MULTI_SWITCH )) {
                    MultiSwitchPacket packet;

                    //fprintf(stderr,"SWITCH %u\n",setup.process_index);

                    Receive( packet );

                    if (rand()&1) {
                         layer.SwitchContext( (DFBBoolean) (rand()&1) );
                    }

                    if (rand()&1) {
                         layer.SetCooperativeLevel( DLSCL_EXCLUSIVE );

                         if (rand()&1)
                              layer.SetCooperativeLevel( DLSCL_SHARED );
                    }

                    if (rand()&1) {
                         layer.SwitchContext( (DFBBoolean) (rand()&1) );
                    }

                    PostRandomSlave( packet );
               }
          }


          postMasterExit();

          return RESULT_SUCCESS;
     }


     class MultiSwitchPacket : public Packet
     {
     public:
          enum {
               TYPE_MULTI_SWITCH = _TYPE_SUPER_PACKET_BASE + 0,
          };

          MultiSwitchPacket()
               :
               Packet( TYPE_MULTI_SWITCH )
          {
               length = sizeof(*this);
          }
     };
};


INSIGNIA_TEST_STATICS( DFBTestMultiSaWMan );

