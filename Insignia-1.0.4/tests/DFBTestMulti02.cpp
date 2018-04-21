#include <++dfb.h>

#include <InsigniaException.h>
#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestMulti02 : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestMulti02 );

private:
     IDirectFB            m_dfb;

     IDirectFBSurface     surface;
     IDirectFBEventBuffer events;

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
     struct ConfigMulti02 : public Config {
          typedef enum {
               DISPLAY_NONE,
               DISPLAY_SIMPLE,
               DISPLAY_CLIENT,
          } DisplayMode;

          DisplayMode  display_mode;
          bool         get_frametime_master;
          bool         get_frametime_slaves;
          unsigned int buffers_master;
          unsigned int buffers_slaves;
          bool         windowed;

          ConfigMulti02( unsigned int num_processes, long long duration,
                         DisplayMode display_mode,
                         bool get_frametime_master, bool get_frametime_slaves,
                         unsigned int buffers_master, unsigned int buffers_slaves,
                         bool windowed )
               :
               display_mode( display_mode ),
               get_frametime_master( get_frametime_master ),
               get_frametime_slaves( get_frametime_slaves ),
               buffers_master( buffers_master ),
               buffers_slaves( buffers_slaves ),
               windowed( windowed )
          {
               this->num_processes = num_processes;
               this->duration      = duration;
          }

          virtual void Describe( Direct::String &string ) const;
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          //for (bool w=false; ;w=!w) {
          bool w = true;
               for (unsigned int i=2; i<=10; i+=4) {
                    for (unsigned int bs=1; bs<=3; bs++) {
                         configs.push_back( new ConfigMulti02( i, 1000000, ConfigMulti02::DISPLAY_NONE, false, false, 1, bs, w ));
                    }

                    for (unsigned int bm=1; bm<=3; bm++) {
                         for (unsigned int bs=1; bs<=3; bs++) {
                              configs.push_back( new ConfigMulti02( i, 1000000, ConfigMulti02::DISPLAY_SIMPLE, false, false, bm, bs, w ));
                         }
                    }

                    for (unsigned int bm=1; bm<=3; bm++) {
                         for (unsigned int bs=1; bs<=3; bs++) {
                              configs.push_back( new ConfigMulti02( i, 1000000, ConfigMulti02::DISPLAY_CLIENT, false, false, bm, bs, w ));
                              //configs.push_back( new ConfigMulti02( i, 1000000, ConfigMulti02::DISPLAY_CLIENT, false, true, bm, bs, w ));
                              //configs.push_back( new ConfigMulti02( i, 1000000, ConfigMulti02::DISPLAY_CLIENT, true, false, bm, bs, w ));
                              //configs.push_back( new ConfigMulti02( i, 1000000, ConfigMulti02::DISPLAY_CLIENT, true, true, bm, bs, w ));
                         }
                    }
               }

//               if (w)
//                    break;
//          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestMulti02";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& configuration ) {
          const ConfigMulti02& config = dynamic_cast<const ConfigMulti02&>(configuration);

          if (setup.process_index > 0) {
               DFBSurfaceDescription desc;

               desc.flags  = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH  |
                                                           DSDESC_HEIGHT |
                                                           DSDESC_CAPS );
               desc.width  = 100;
               desc.height = 100;
               desc.caps   = (DFBSurfaceCapabilities)( DSCAPS_SHARED | Insignia::Util::BufferCaps(config.buffers_slaves) );

               surface     = m_dfb.CreateSurface( desc );

               surface.AllowAccess( "/*" );

               // Send surface (ID) to master test instance
               SurfaceIDPacket packet( surface.GetID() );

               Post( 0, packet );
          }
          else if (config.display_mode != ConfigMulti02::DISPLAY_NONE) {
               if (!config.windowed)
                    m_dfb.SetCooperativeLevel( DFSCL_FULLSCREEN );


               DFBSurfaceDescription desc;

               desc.flags  = (DFBSurfaceDescriptionFlags)( DSDESC_CAPS );
               desc.caps   = (DFBSurfaceCapabilities)( DSCAPS_PRIMARY | Insignia::Util::BufferCaps(config.buffers_master) );

               surface     = m_dfb.CreateSurface( desc );

               if (config.display_mode == ConfigMulti02::DISPLAY_CLIENT)
                    events = m_dfb.CreateEventBuffer();
          }
     }


     virtual Result Run( const Config& configuration ) {
          const ConfigMulti02& config = dynamic_cast<const ConfigMulti02&>(configuration);

          if (setup.process_index == 0)
               return runMaster( config );

          return runSlave( config );
     }

     virtual void Cleanup( const Config& config ) {
          surface = NULL;
          events  = NULL;
     }

     virtual void Finalize() {
          m_dfb = NULL;

          if (setup.process_index > 0) {
               PacketExit packet;

               Post( 0, packet );
          }
     }


private:
     class SurfaceHolder {
     public:
          IDirectFBSurface surface;
          DFBRectangle     dest;
          DFBSurfaceEvent  last;

          SurfaceHolder()
          {
          }

          void handleUpdate( const DFBSurfaceEvent &event,
                             const Setup           &setup )
          {
               if (last.type) {
                    if (last.flip_count && last.flip_count != event.flip_count - 1)
                         throw new Insignia::Exception( "Invalid flip count %u after %u", event.flip_count, last.flip_count );

                    //if (setup.config->get_frametime_slaves) {
                         
                   // }
               }

               surface.iface->FrameAck( surface.iface, event.flip_count );

               last = event;
          }

          inline SurfaceHolder& operator= (IDirectFBSurface &surface) {
               this->surface   = surface;
               this->last.type = DSEVT_NONE;
               return *this;
          }
     };

     Result runMaster( const ConfigMulti02& config ) {
          unsigned int                         num_slaves = config.num_processes - 1;
          std::map<DFBSurfaceID,SurfaceHolder> sources;
          unsigned int                         n = 0;

          // Receive surfaces (IDs) from slave test instances
          while (sources.size() < num_slaves) {
               SurfaceIDPacket packet;

               Receive( packet );

               D_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Got surface ID %u from %u\n",
                       packet.surface_id, packet.sender );

               IDirectFBSurface surface = m_dfb.GetSurface( packet.surface_id );

               if (config.display_mode == ConfigMulti02::DISPLAY_CLIENT) {
                    surface.iface->AttachEventBuffer( surface.iface, events.iface );
                    surface.iface->MakeClient( surface.iface );
               }

               sources[packet.surface_id]      = surface;
               sources[packet.surface_id].dest = DFBRectangle( (n % 3) * 100, (n / 3) * 100, 100, 100 );

               n++;
          }

          receiveSlavesGo();
          postSlavesGo();

          switch (config.display_mode) {
               case ConfigMulti02::DISPLAY_NONE:
                    receiveSlavesExit();
                    break;

               case ConfigMulti02::DISPLAY_SIMPLE:
                    do {
                         if (config.get_frametime_master)
                              surface.iface->GetFrameTime( surface.iface, 0 );

                         surface.Clear();

                         for (std::map<DFBSurfaceID,SurfaceHolder>::iterator it = sources.begin();
                              it != sources.end();
                              it++)
                         {
                              SurfaceHolder &holder = (*it).second;

                              surface.StretchBlit( holder.surface, NULL, &holder.dest );
                         }

                         surface.Flip();
                    } while (checkSlavesExit());
                    break;

               case ConfigMulti02::DISPLAY_CLIENT:
                    do {
                         while (events.WaitForEventWithTimeout( 0, 100 )) {
                              DFBEvent event;

                              while (events.GetEvent( &event )) {
                                   switch (event.clazz) {
                                        case DFEC_SURFACE:
                                             switch (event.surface.type) {
                                                  case DSEVT_UPDATE:
                                                       sources[event.surface.surface_id].handleUpdate( event.surface, setup );
                                                       break;

                                                  default:
                                                       break;
                                             }
                                             break;

                                        default:
                                             break;
                                   }
                              }
                         }

                         if (config.get_frametime_master)
                              surface.iface->GetFrameTime( surface.iface, 0 );

                         surface.Clear();

                         for (std::map<DFBSurfaceID,SurfaceHolder>::iterator it = sources.begin();
                              it != sources.end();
                              it++)
                         {
                              SurfaceHolder &holder = (*it).second;

                              surface.StretchBlit( holder.surface, NULL, &holder.dest );
                         }

                         surface.Flip();
                    } while (checkSlavesExit());
                    break;

               default:
                    D_BUG( "unknown display mode 0x%x", config.display_mode );
          }

          return RESULT_SUCCESS;
     }


     Result runSlave( const ConfigMulti02& config ) {
          postMasterGo();
          receiveGo();


          int x = 0;

          do {
               if (config.get_frametime_slaves) {
                    long long micros;

                    surface.iface->GetFrameTime( surface.iface, &micros );

                    x = micros / 10000;
               }
               else
                    x++;

               surface.Clear( setup.process_index * 0x1234, setup.process_index * 0x2345, setup.process_index * 0x3456, setup.process_index * 0x4567 );

               surface.SetColor( 0xff, 0xff, 0xff, 0xff );
               surface.FillRectangle( x % (100-10), 10, 10, 100-20 );

               surface.Flip();
          } while (!stopped());


          postMasterExit();

          return RESULT_SUCCESS;
     }
};



template <>
ToString<DFBTestMulti02::ConfigMulti02::DisplayMode>::ToString( const DFBTestMulti02::ConfigMulti02::DisplayMode &display_mode )
{
     switch (display_mode) {
          case DFBTestMulti02::ConfigMulti02::DISPLAY_NONE:
               PrintF( "DISPLAY_NONE" );
               break;

          case DFBTestMulti02::ConfigMulti02::DISPLAY_SIMPLE:
               PrintF( "DISPLAY_SIMPLE" );
               break;

          case DFBTestMulti02::ConfigMulti02::DISPLAY_CLIENT:
               PrintF( "DISPLAY_CLIENT" );
               break;

          default:
               PrintF( "invalid display mode 0x%x", display_mode );
     }
}

void
DFBTestMulti02::ConfigMulti02::Describe( Direct::String &string ) const
{
     Config::Describe( string );

     string.PrintF( "  || %-14s GetFrameTime(%c%c) buffers(%u,%u) %s",
                    ToString<DisplayMode>( display_mode ).buffer(),
                    get_frametime_master ? 'M' : '.',
                    get_frametime_slaves ? 'S' : '.',
                    buffers_master, buffers_slaves,
                    windowed ? "windowed" : "fullscreen" );
}


INSIGNIA_TEST_STATICS( DFBTestMulti02 );

