#ifndef __INSIGNIA__TEST_H__
#define __INSIGNIA__TEST_H__

#include "common.h"

#include <list>
#include <map>
#include <vector>

#include <direct/LockWQ.h>
#include <direct/String.h>

#include "InsigniaUtil.h"


namespace Insignia {

class App;

class Test {
public:
     Test()
          :
          packets_reading( false )
     {
          memset( slave_exit, 0, sizeof(slave_exit) );
     }

     virtual ~Test() {
     }

     class Config {
     public:
          static std::string nullString;

          std::vector<std::pair<std::string*,std::string*> > dfb_options;
          std::vector<std::pair<std::string*,std::string*> > dfb_options_master;
          std::vector<std::pair<std::string*,std::string*> > dfb_options_slaves;
          unsigned int                                       num_processes;
          long long                                          duration;


          Config()
               :
               num_processes( 1 ),
               duration( 500000 )
          {
          }

          Config( const Direct::String &config )
               :
               num_processes( 1 ),
               duration( 0 )
          {
               Direct::Strings options = config.GetTokens( "," );

               for (Direct::Strings::const_iterator it = options.begin(); it != options.end(); it++) {
                    Direct::Strings name_val = (*it).GetTokens( "=" );

                    D_ASSUME( name_val.size() >= 1 );
                    D_ASSUME( name_val.size() <= 2 );

                    switch (name_val.size()) {
                         case 1:
                              AddOption( name_val[0], Test::Config::nullString );
                              break;

                         case 2:
                              AddOption( name_val[0], name_val[1] );
                              break;
                    }
               }
          }

          /*
           * Description
           */

          virtual void Describe( Direct::String &string ) const {
               string.PrintF( "[%u] %s (%lld.%03lldms)", num_processes,
                              PrintOptions().c_str(), duration/1000LL, duration%1000LL );
          }

          Direct::String GetDescription() const {
               Direct::String string;

               Describe( string );

               return string;
          }

          /*
           * DirectFB Options
           */

          void AddOption( const std::string &name,
                          const std::string &value = nullString )
          {
               dfb_options.push_back( std::pair<std::string*,std::string*>( new std::string(name),
                                                                            &value != &nullString ? new std::string(value) : NULL ) );
          }

          void AddOptionMaster( const std::string &name,
                                const std::string &value = nullString )
          {
               dfb_options_master.push_back( std::pair<std::string*,std::string*>( new std::string(name),
                                                                                   &value != &nullString ? new std::string(value) : NULL ) );
          }

          void AddOptionSlaves( const std::string &name,
                                const std::string &value = nullString )
          {
               dfb_options_slaves.push_back( std::pair<std::string*,std::string*>( new std::string(name),
                                                                                   &value != &nullString ? new std::string(value) : NULL ) );
          }

          void ApplyOptions( bool master ) const
          {
               for (std::vector<std::pair<std::string*,std::string*> >::const_iterator it = dfb_options.begin();
                    it != dfb_options.end(); it++)
               {
                    DirectFBSetOption( (*it).first->c_str(), (*it).second ? (*it).second->c_str() : NULL );
               }

               if (master) {
                    for (std::vector<std::pair<std::string*,std::string*> >::const_iterator it = dfb_options_master.begin();
                         it != dfb_options_master.end(); it++)
                    {
                         DirectFBSetOption( (*it).first->c_str(), (*it).second ? (*it).second->c_str() : NULL );
                    }
               }
               else {
                    for (std::vector<std::pair<std::string*,std::string*> >::const_iterator it = dfb_options_slaves.begin();
                         it != dfb_options_slaves.end(); it++)
                    {
                         DirectFBSetOption( (*it).first->c_str(), (*it).second ? (*it).second->c_str() : NULL );
                    }
               }
          }

          std::string PrintOptions() const
          {
               Direct::String ret;

               for (std::vector<std::pair<std::string*,std::string*> >::const_iterator it = dfb_options.begin();
                    it != dfb_options.end(); it++)
               {
                    if ((*it).second)
                         ret.PrintF( " (%-20s=%-10s)", (*it).first->c_str(), (*it).second->c_str() );
                    else
                         ret.PrintF( " (%-20s)", (*it).first->c_str() );
               }

               return ret;
          }

          std::string PrintOptionsMaster() const
          {
               Direct::String ret;

               for (std::vector<std::pair<std::string*,std::string*> >::const_iterator it = dfb_options_master.begin();
                    it != dfb_options_master.end(); it++)
               {
                    if ((*it).second)
                         ret.PrintF( " (%-20s=%-10s)", (*it).first->c_str(), (*it).second->c_str() );
                    else
                         ret.PrintF( " (%-20s)", (*it).first->c_str() );
               }

               return ret;
          }

          std::string PrintOptionsSlaves() const
          {
               Direct::String ret;

               for (std::vector<std::pair<std::string*,std::string*> >::const_iterator it = dfb_options_slaves.begin();
                    it != dfb_options_slaves.end(); it++)
               {
                    if ((*it).second)
                         ret.PrintF( " (%-20s=%-10s)", (*it).first->c_str(), (*it).second->c_str() );
                    else
                         ret.PrintF( " (%-20s)", (*it).first->c_str() );
               }

               return ret;
          }
     };

     class Info {
          public:
          Info() : name("<unnamed>") {
          }

          string name;
     };

     typedef enum {
          RESULT_SUCCESS,     // success, returned by Run()
          RESULT_FALLBACK,    // fallback used, returned by Run()
          RESULT_FAILURE,     // failing test, returned by Run()

          RESULT_SIGNAL,      // signal during Run()
          RESULT_SEGFAULT,    // segfault during Run()
          RESULT_EXCEPTION,   // exception during Run()

          RESULT_SHUTDOWN,    // exception/signal during Cleanup() or Shutdown()
          RESULT_PREPARE,     // exception/signal during Prepare()
          RESULT_INITIALISE,  // exception/signal during Initialise()
     } Result;


public:
     virtual void GetConfigs( vector<Config*> &configs ) = 0;
     virtual void GetInfo( Info &info ) = 0;

public:
     virtual void Initialize( unsigned int process_index ) {
     }

     virtual void Finalize() {
     }

public:
     virtual void Prepare( const Config& config ) {
     }

     virtual Result Run( const Config& config ) = 0;

     virtual void Cleanup( const Config& config ) {
     }



private:
     int  fdin;
     int *fdout;

public:
     class Status {
     public:
          Result    result;
          int       signal;
          char      message[200];

          Status( Result                result  = RESULT_SUCCESS,
                  int                   signal  = 0,
                  const Direct::String &message = "" )
               :
               result( result ),
               signal( signal )
          {
               direct_snputs( this->message, message.buffer(), sizeof(this->message) );
          }

          inline Status &operator =(const Direct::String &string )
          {
               direct_snputs( this->message, string.buffer(), sizeof(this->message) );
               return *this;
          }
     };

     class Setup {
     public:
          typedef enum {
               FLAGS_NONE     = 0x00000000
          } Flags;

          App                 *app;
          vector<Test::Config*> *configs;
          const Test::Config *config;
          unsigned int        config_index;
          long long           testrun;
          int                 process_index;
          const int          *fds;
          Flags               flags;

          Setup()
               :
               app( NULL ),
               configs( NULL ),
               config( NULL ),
               config_index( 0 ),
               testrun( 0 ),
               process_index( 0 ),
               fds( NULL ),
               flags( FLAGS_NONE )
          {
          }

          Setup( App                   *app,
                 vector<Test::Config*> &configs,
                 unsigned int           config_index,
                 int                    process_index,
                 long long              testrun,
                 const int             *fds )
               :
               app( app ),
               configs( &configs ),
               config( configs[config_index] ),
               config_index( config_index ),
               testrun( testrun ),
               process_index( process_index ),
               fds( fds ),
               flags( FLAGS_NONE )
          {
          }
     };

     virtual void DoSetup( const Setup &setup )
     {
          fdin = setup.fds[setup.process_index*2+2];

          fdout = new int[setup.config->num_processes+1];

          for (unsigned int i=0; i<setup.config->num_processes+1; i++)
               fdout[i] = setup.fds[i*2+1];

          this->setup = setup;

          us_start = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );

          num_slaves = setup.config->num_processes - 1;
     }

protected:
     Setup        setup;
     long long    us_start;
     unsigned int num_slaves;
     bool         slave_exit[999];

     bool stopped()
     {
          if (setup.config->duration)
               return (direct_clock_get_time( DIRECT_CLOCK_MONOTONIC ) - us_start >= setup.config->duration);

          return GotPacket( Packet::TYPE_STOP );
     }



protected:
     std::vector<Direct::String>   extra_files;

     Direct::String getExtraFileName( const Direct::String &filename )
     {
          Test::Info info;

          GetInfo( info );// FIXME


          Direct::String name = Direct::String::F( "%012lld/%s__%d_%012lld.%s",
                                                   setup.testrun, info.name.c_str(),
                                                   setup.process_index, setup.testrun,
                                                   filename.buffer() );

          //direct_log_printf( NULL, "# new extra file name '%s'\n", name.buffer() );

          return name;
     }

     void addExtraFile( const Direct::String &filename )
     {
          if (setup.process_index == -1) {
               //direct_log_printf( NULL, "# add extra file '%s' locally\n", filename.buffer() );

               extra_files.push_back( filename );
          }
          else {
               //direct_log_printf( NULL, "# add extra file '%s' at main (post)\n", filename.buffer() );

               PacketUnlink unlink( filename );

               PostMain( unlink );
          }
     }

     Direct::String getAndAddExtraFile( const Direct::String &filename )
     {
          Direct::String name = getExtraFileName( filename );

          addExtraFile( name );

          return name;
     }

     void dumpSurface( IDirectFBSurface     &surface,
                       const Direct::String &name )
     {
          Direct::String filebase = getExtraFileName( name );

          addExtraFile( filebase + ".ppm" );

          if (DFB_PIXELFORMAT_HAS_ALPHA( surface.GetPixelFormat() ))
               addExtraFile( filebase + ".pgm" );

          surface.Dump( filebase.buffer(), NULL );
     }

public:
     void unlinkExtraFiles()
     {
          //direct_log_printf( NULL, "# unlinking extra files...\n" );

          for (std::vector<Direct::String>::const_iterator it=extra_files.begin(); it!=extra_files.end(); it++) {
               const Direct::String &name = *it;

               //direct_log_printf( NULL, "# unlink '%s'\n", name.buffer() );

               int ret = unlink( name.buffer() );
               if (ret < 0)
                    D_ERROR( "Insignia: unlink( %s ) failed (%s)!\n", name.buffer(), strerror(errno) );
          }

          extra_files.clear();

          while (GotPacket( Packet::TYPE_UNLINK )) {
               PacketUnlink packet;

               Receive( packet );

               //direct_log_printf( NULL, "# unlink '%s' (%d)\n", packet.filename, packet.sender );

               int ret = unlink( packet.filename );
               if (ret < 0)
                    D_ERROR( "Insignia: unlink( %s ) failed (%s)!\n", packet.filename, strerror(errno) );
          }
     }

public:
     class Packet {
     public:
          u32  length;
          s32  sender;
          u32  type;

          enum {
               TYPE_INVALID  = 0,

               TYPE_GO       = 1,  // used by tests intermittedly and in any order/directions
               TYPE_EXIT     = 2,  // used by slaves to master (waiting/checking til slaves' stopped() has returned true)

               TYPE_STOP     = 3,  // sent from main to processes in user stop mode (stopped() returns true)
               TYPE_FINALISE = 4,  // sent from slaves to master on normal exit, otherwise main's wait() loop does it
               TYPE_UNLINK   = 5,  // sent from processes to main to add a file to the extra file list (for deletion upon successful run)
               TYPE_STATUS   = 6,  // sent from processes to main to update about current stage (init, run, shutdown, exception...)

               _TYPE_SUPER_PACKET_BASE = 1000
          };

          Packet( u32 type )
               :
               length( sizeof(*this) ),
               sender( 0 ),
               type( type )
          {
          }
     };

     class PacketGo : public Packet {
     public:
          PacketGo() : Packet( TYPE_GO ) {}
     };

     class PacketExit : public Packet {
     public:
          PacketExit() : Packet( TYPE_EXIT ) {}
     };

     class PacketStop : public Packet {
     public:
          PacketStop() : Packet( TYPE_STOP ) {}
     };

     class PacketFinalise : public Packet {
     public:
          PacketFinalise() : Packet( TYPE_FINALISE ) {}
     };

     class PacketUnlink : public Packet {
     public:
          PacketUnlink( const Direct::String &filename = "" ) : Packet( TYPE_UNLINK )
          {
               length = sizeof(*this);

               if (filename.length() > sizeof(this->filename)-1)
                    D_WARN( "filename '%s' too long", filename.buffer() );

               direct_snputs( this->filename, filename.buffer(), sizeof(this->filename) );
          }

          char filename[2000];
     };

     class PacketStatus : public Packet {
     public:
          Status status;

          PacketStatus( const Status &status = Status() )
               :
               Packet( TYPE_STATUS ),
               status( status )
          {
               length = sizeof(*this);
          }
     };


     bool Receive( Packet &packet,
                   bool    interruptible = false );

     bool GotPacket( u32 type );

     void Post( int     process_index,
                Packet &packet );

     void PostAs( int     process_index,
                  Packet &packet,
                  int     sender );

     void PostMain( Packet &packet );
     void PostMainAs( Packet &packet, int sender );
     void PostAll( Packet &packet );
     void PostAllSlaves( Packet &packet );
     void ReceiveAll( Packet &packet );
     void ReceiveAllSlaves( Packet &packet );

     void PostRandomSlave( Packet &packet );

     void PostStatus( const Status &status );
     void PostStatusAs( const Status &status, int sender );


     void receiveGo()
     {
          PacketGo packet;

          Receive( packet );

          D_INFO( "Insignia/Test: --------------------- received GO (%u <- %u) ---------------------\n", setup.process_index, packet.sender );
     }

     void receiveSlavesGo()
     {
          PacketGo packet;

          ReceiveAllSlaves( packet );
     }

     void receiveSlavesExit()
     {
          for (unsigned int i=0; i<num_slaves; i++) {
               if (!slave_exit[i]) {
                    PacketExit packet;

                    //fprintf( stderr, ">>>>>>>>>>>>>>>> no exit from %u...\n", i );
                    if (Receive( packet, true )) {
                         //fprintf( stderr, ">>>>>>>>>>>>>>>> got exit from %u...\n", packet.sender );
                         D_INFO( "Insignia/Test: --------------------- received EXIT (%u <- %u) ---------------------\n", setup.process_index, packet.sender );

                         D_ASSERT( num_slaves > 0 );
                         D_ASSERT( packet.sender <= (s32) num_slaves );

                         slave_exit[packet.sender-1] = true;
                    }

                    i = -1;
               }
          }
     }

     /*
      * Returns true as long as slaves are running
      */
     bool checkSlavesExit()
     {
          if (GotPacket( Packet::TYPE_EXIT )) {
               PacketExit packet;

               Receive( packet );

               D_INFO( "Insignia/Test: --------------------- received EXIT (%u <- %u) ---------------------\n", setup.process_index, packet.sender );

               D_ASSERT( num_slaves > 0 );
               D_ASSERT( packet.sender <= (s32) num_slaves );

               slave_exit[packet.sender-1] = true;
          }

          for (unsigned int i=0; i<num_slaves; i++) {
               if (!slave_exit[i])
                    return true;
          }

          return false;
     }

     void postMasterGo()
     {
          PacketGo packet;

          Post( 0, packet );
     }

     void postMasterExit()
     {
          PacketExit packet;

          Post( 0, packet );
     }

     void postSlavesGo()
     {
          D_INFO( "Insignia/Test: --------------------- posting GO to all %u slaves ---------------------\n", setup.config->num_processes - 1 );

          PacketGo packet;

          PostAllSlaves( packet );
     }

protected:
     std::map<u32,std::list<Packet*> >  packets;
     Direct::LockWQ                     packets_lwq;
     bool                               packets_reading;

private:
     void readPacket();

protected:
     class Register {
          public:
          Register( Test &test );
     };
};


}


#define INSIGNIA_TEST_CLASS( TestClass )     \
private:                                     \
     static Register  _test_register;        \
     static TestClass _test_instance;


#define INSIGNIA_TEST_STATICS( TestClass )                                      \
TestClass                TestClass::_test_instance;                               \
Insignia::Test::Register TestClass::_test_register( TestClass::_test_instance );


#endif

