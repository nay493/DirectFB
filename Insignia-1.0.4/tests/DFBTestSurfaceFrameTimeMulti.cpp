#include <++dfb.h>

#include <InsigniaException.h>
#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestSurfaceFrameTimeMulti : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestSurfaceFrameTimeMulti );

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
     struct ConfigSurfaceFrameTime : public Config {
          ConfigSurfaceFrameTime( unsigned int num_processes,
                                  long long    interval,
                                  long long    max_advance,
                                  unsigned int num_buffers )
               :
               interval( interval ),
               max_advance( max_advance ),
               num_buffers( num_buffers )
          {
               this->num_processes = num_processes;
          }

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );

               string.PrintF( "  || Interval: %lld  MaxAdvance %lld  NumBuffers %u", interval, max_advance, num_buffers );
          }

          long long    interval;
          long long    max_advance;
          unsigned int num_buffers;
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          long long    interval[]      = { 16666, 20000, 25000 };
          long long    max_advance[]   = { 100000, 200000 };
          unsigned int num_processes[] = { 2, 3, 4, 7, 11, 17, 28 };
          unsigned int num_buffers[]   = { 1, 2, 3 };

          for (int i=0; i<D_ARRAY_SIZE(interval); i++) {
               for (int m=0; m<D_ARRAY_SIZE(max_advance); m++) {
                    for (int n=0; n<D_ARRAY_SIZE(num_processes); n++) {
                         for (int b=0; b<D_ARRAY_SIZE(num_buffers); b++) {
                              configs.push_back( new ConfigSurfaceFrameTime( num_processes[n], interval[i], max_advance[m], num_buffers[b] ));
                         }
                    }
               }
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestSurfaceFrameTimeMulti";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& configuration ) {
          const ConfigSurfaceFrameTime& config = dynamic_cast<const ConfigSurfaceFrameTime&>(configuration);

          if (setup.process_index > 0) {
               DFBSurfaceDescription desc;

               desc.flags  = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS );
               desc.width  = 16;
               desc.height = 16;
               desc.caps   = Insignia::Util::BufferCaps( config.num_buffers );

               surface = m_dfb.CreateSurface( desc );

               surface.AllowAccess( "/*" );

               // Send surface (ID) to master test instance
               SurfaceIDPacket packet( surface.GetID() );

               Post( 0, packet );
          }
          else
               events = m_dfb.CreateEventBuffer();
     }


     virtual Result Run( const Config& configuration ) {
          const ConfigSurfaceFrameTime &config = dynamic_cast<const ConfigSurfaceFrameTime&>(configuration);

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
     }

private:
     class SurfaceHolder {
     public:
          IDirectFBSurface   surface;
          DFBRectangle       dest;
          DFBFrameTimeConfig ftc;
          DFBSurfaceEvent    first;
          DFBSurfaceEvent    last;

          SurfaceHolder()
          {
          }

          void handleUpdate( const DFBSurfaceEvent &event,
                             const Setup           &setup )
          {
               long long now     = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );
               long long advance = event.time_stamp - now;

               if (advance > ftc.max_advance)
                    throw new Insignia::Exception( "Too much in advance (%lld > %lld)", advance, ftc.max_advance );

               if (last.type) {
                    if (last.flip_count && last.flip_count != event.flip_count - 1)
                         throw new Insignia::Exception( "Invalid flip count %u after %u", event.flip_count, last.flip_count );

                    if (advance < -ftc.interval * 2) {
                         //throw new Insignia::Exception( "More than two intervals in past (%lld)", advance );
                         direct_log_printf( NULL, "More than two intervals in past (%lld)", advance );
                    }

                    if (last.time_stamp) {
                         long long interval = event.time_stamp - last.time_stamp;
     
                         if (interval < 0)
                              throw new Insignia::Exception( "Negative interval %lld (%lld - %lld)", interval, event.time_stamp, last.time_stamp );
     
                         long long diff = ABS(interval - ftc.interval);
     
                         if (diff > ftc.interval * 100)
                              throw new Insignia::Exception( "Interval %lld (%lld - %lld) differs too much (%lld > %lld)",
                                                             interval, last.time_stamp, event.time_stamp, diff, ftc.interval * 100 );
                    }
               }

               surface.iface->FrameAck( surface.iface, event.flip_count );

               if (event.time_stamp) {
                    last = event;
     
                    if (!first.type)
                         first = event;
               }
          }

          inline SurfaceHolder& operator= (IDirectFBSurface &surface) {
               this->surface    = surface;
               this->first.type = DSEVT_NONE;
               this->last.type  = DSEVT_NONE;
               return *this;
          }
     };

     virtual Result runMaster( const ConfigSurfaceFrameTime& config ) {
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

               surface.iface->AttachEventBuffer( surface.iface, events.iface );
               surface.iface->MakeClient( surface.iface );


               DFBFrameTimeConfig ftc;

               ftc.flags       = (DFBFrameTimeConfigFlags)(DFTCF_INTERVAL | DFTCF_MAX_ADVANCE);
               ftc.interval    = config.interval;
               ftc.max_advance = config.max_advance;

               surface.iface->SetFrameTimeConfig( surface.iface, &ftc );


               sources[packet.surface_id]      = surface;
               sources[packet.surface_id].dest = DFBRectangle( (n % 3) * 100, (n / 3) * 100, 100, 100 );
               sources[packet.surface_id].ftc  = ftc;

               n++;
          }

          receiveSlavesGo();
          postSlavesGo();

          long long start = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );

          while (checkSlavesExit()) {
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
          }

          for (std::map<DFBSurfaceID,SurfaceHolder>::iterator it = sources.begin();
               it != sources.end();
               it++)
          {
               SurfaceHolder &holder = (*it).second;

               long long first = holder.first.type ? holder.first.time_stamp : 0;
               long long last  = holder.last.type  ? holder.last.time_stamp  : 0;
               long long now   = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );

               if (last - first > now - start) {
                    direct_log_printf( NULL, "Invalid source time span %lld > actual %lld (source %lld - %lld, actual %lld - %lld)\n",
                                       last - first, now - start, first, last, start, now );

                    return RESULT_FAILURE;
               }
          }

          return RESULT_SUCCESS;
     }

     virtual Result runSlave( const ConfigSurfaceFrameTime& config ) {
          postMasterGo();
          receiveGo();


          int       i;
          long long start = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );

          for (i=0; i<100 || !stopped(); i++) {
               long long micros;

               surface.iface->GetFrameTime( surface.iface, &micros );

               long long now = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );

               if (micros - now > config.max_advance) {
                    direct_log_printf( NULL, "TEST FAILED (%lld in advance with max at %lld, micros %lld, now %lld, interval %lld)!\n",
                                       micros - now, config.max_advance, micros, now, config.interval );

                    return RESULT_FAILURE;
               }

               surface.Clear();

               surface.Flip();
          }

          long long last = (i-1) * (config.interval);
          long long now  = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );

          if (now - start > last + config.interval * 100) {
               direct_log_printf( NULL, "TEST FAILED (end of run >%lld.%03lldms ahead, %lld.%03lldms instead of %lld.%03lldms)!\n",
                                  PRINT_US_MS(config.interval * 100), PRINT_US_MS(now - start), PRINT_US_MS(last) );

               return RESULT_FAILURE;
          }

          postMasterExit();

          return RESULT_SUCCESS;
     }
};


INSIGNIA_TEST_STATICS( DFBTestSurfaceFrameTimeMulti );

