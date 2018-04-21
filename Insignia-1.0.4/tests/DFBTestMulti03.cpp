#include <++dfb.h>

#include <InsigniaException.h>
#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestMulti03 : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestMulti03 );

private:
     IDirectFB            m_dfb;

     IDirectFBSurface     surface;

     class SurfaceIDPacket : public Packet
     {
     public:
          enum {
               TYPE_SURFACE_ID = _TYPE_SUPER_PACKET_BASE + 0,
          };

          DFBSurfaceID surface_id;

          SurfaceIDPacket( DFBSurfaceID surface_id = 0 )
               :
               Packet( TYPE_SURFACE_ID ),
               surface_id( surface_id )
          {
               length = sizeof(*this);
          }
     };

public:
     struct ConfigMulti03 : public Config {
          unsigned int buffers;
          bool         windowed;

          ConfigMulti03( unsigned int num_processes, long long duration,
                         unsigned int buffers,
                         bool windowed )
               :
               buffers( buffers ),
               windowed( windowed )
          {
               this->num_processes = num_processes;
               this->duration      = duration;
          }

          virtual void Describe( Direct::String &string ) const;
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          for (bool w=false; ;w=!w) {
               for (unsigned int i=2; i<=10; i++) {
                    for (unsigned int bs=1; bs<=3; bs++) {
                         configs.push_back( new ConfigMulti03( i, 1000000, bs, w ));
                    }
               }

               if (w)
                    break;
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestMulti03";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& configuration ) {
          const ConfigMulti03& config = dynamic_cast<const ConfigMulti03&>(configuration);

          if (setup.process_index == 1) {
               if (!config.windowed)
                    m_dfb.SetCooperativeLevel( DFSCL_FULLSCREEN );


               DFBSurfaceDescription desc;

               desc.flags  = (DFBSurfaceDescriptionFlags)( DSDESC_CAPS );
               desc.caps   = (DFBSurfaceCapabilities)( DSCAPS_PRIMARY | Insignia::Util::BufferCaps(config.buffers) );

               surface     = m_dfb.CreateSurface( desc );

               surface.AllowAccess( "*" );
          }
     }


     virtual Result Run( const Config& configuration ) {
          const ConfigMulti03& config = dynamic_cast<const ConfigMulti03&>(configuration);

          if (setup.process_index == 0)
               return runMaster( config );

          return runSlave( config );
     }

     virtual void Cleanup( const Config& config ) {
          surface = NULL;
     }

     virtual void Finalize() {
          m_dfb = NULL;

          if (setup.process_index > 0) {
               PacketExit packet;

               Post( 0, packet );
          }
     }


private:
     Result runMaster( const ConfigMulti03& config ) {
          receiveSlavesGo();
          postSlavesGo();

          receiveSlavesExit();

          return RESULT_SUCCESS;
     }


     Result runSlave( const ConfigMulti03& config ) {
          postMasterGo();
          receiveGo();

          if (setup.process_index == 1) {
               // Send surface (ID) to slave test instances
               SurfaceIDPacket packet( surface.GetID() );
     
               PostAllSlaves( packet );
          }

          // Receive surface (ID) from master test instance
          SurfaceIDPacket packet;

          Receive( packet );

          IDirectFBSurface surface = m_dfb.GetSurface( packet.surface_id );

          DFBSurfaceDescription desc;

          desc.flags       = DSDESC_PIXELFORMAT;
          desc.pixelformat = surface.GetPixelFormat();

          IDirectFBSurface dest = m_dfb.CreateSurface( desc );

          int x = 0;

          do {
               x++;

               surface.Clear( setup.process_index * 0x1234, setup.process_index * 0x2345, setup.process_index * 0x3456, setup.process_index * 0x4567 );

               surface.SetColor( 0xff, 0xff, 0xff, 0xff );
               surface.FillRectangle( x % (100-10), 10, 10, 100-20 );

               dest.Blit( surface );
               dest.ReleaseSource();

               surface.Flip();
          } while (!stopped());


          postMasterExit();

          return RESULT_SUCCESS;
     }
};



void
DFBTestMulti03::ConfigMulti03::Describe( Direct::String &string ) const
{
     Config::Describe( string );

     string.PrintF( "  || %u buffers %s",
                    buffers,
                    windowed ? "windowed" : "fullscreen" );
}


INSIGNIA_TEST_STATICS( DFBTestMulti03 );

