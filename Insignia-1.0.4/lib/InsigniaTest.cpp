#include "InsigniaException.h"
#include "InsigniaTest.h"
#include "InsigniaTestManager.h"

namespace Insignia {

std::string Test::Config::nullString;


Test::Register::Register( Test &test )
{
     TestManager::RegisterTest( test );
}


void
Test::Post( int     process_index,
            Packet &packet )
{
     PostAs( process_index, packet, setup.process_index );
}

void
Test::PostAs( int     process_index,
              Packet &packet,
              int     sender )
{
     packet.sender = sender;

     ssize_t ret = write( fdout[process_index+1], &packet, packet.length );

     if (ret < 0)
          D_PERROR( "Insignia::Test::Post: write(%u) failed!\n", packet.length );
     else if ((size_t) ret != packet.length)
          D_ERROR( "Insignia::Test::Post: write(%u) returned only %zd!\n", packet.length, ret );
}

void
Test::PostMain( Packet &packet )
{
     Post( -1, packet );
}

void
Test::PostMainAs( Packet &packet, int sender )
{
     PostAs( -1, packet, sender );
}

void
Test::PostAll( Packet &packet )
{
     for (unsigned int np=0; np<setup.config->num_processes; np++)
          Post( np, packet );
}

void
Test::PostAllSlaves( Packet &packet )
{
     for (unsigned int np=1; np<setup.config->num_processes; np++)
          Post( np, packet );
}

void
Test::ReceiveAll( Packet &packet )
{
     for (unsigned int np=0; np<setup.config->num_processes; np++)
          Receive( packet );
}

void
Test::ReceiveAllSlaves( Packet &packet )
{
     for (unsigned int np=1; np<setup.config->num_processes; np++)
          Receive( packet );
}

void
Test::PostRandomSlave( Packet &packet )
{
     Post( (((u32) rand()) % (setup.config->num_processes-1)) + 1, packet );
}

void
Test::PostStatus( const Status &status )
{
     PacketStatus packet( status );

     PostMain( packet );
}

void
Test::PostStatusAs( const Status &status, int sender )
{
     PacketStatus packet( status );

     PostMainAs( packet, sender );
}

bool
Test::Receive( Packet &packet,
               bool    interruptible )
{
     while (true)
     {
          Direct::LockWQ::Lock l1( packets_lwq );

          if (!packets[packet.type].empty())
          {
               //D_INFO("  receive %u!!\n",packet.type);
               std::list<Packet*>::iterator  it = packets[packet.type].begin();
               Packet                       *p  = *it;

               packets[packet.type].erase( it );

               if (p->length != packet.length)
                    throw new Exception( Direct::String::F( "packet length mismatch (sender %u, receiver %u)", p->length, packet.length ) );

               direct_memcpy( &packet, p, p->length );

               return true;
          }

          if (packets_reading)
               l1.wait();
          else {
               packets_reading = true;
               packets_lwq.unlock();


               fd_set rset;
               int    number_file;

               /* get udev event */
               FD_ZERO(&rset);
               FD_SET(fdin, &rset);

               //fprintf( stderr, ">>>>>>>>>>>>>>>> select...\n" );
               while ((number_file = select(fdin+1, &rset, NULL, NULL, NULL)) < 0) {
                    if (errno != EINTR) {
                         D_PERROR( "Insignia/Test:: select() failed!\n" );
                         return false;
                    }
               }


               packets_lwq.lock();

               if (FD_ISSET( fdin, &rset ))
                    readPacket();

               packets_reading = false;
          }

          //fprintf( stderr, ">>>>>>>>>>>>>>>> inter %d, empty %d\n", interruptible, packets[packet.type].empty() );
          if (interruptible && packets[packet.type].empty())
               break;
     }

     return false;
}

bool
Test::GotPacket( u32 type )
{
     while (true)
     {
          Direct::LockWQ::Lock l1( packets_lwq );

          //D_INFO("check for  %u\n",type);

          if (!packets[type].empty()) {
               //D_INFO("  got %u!!\n",type);
               return true;
          }


          if (!packets_reading) {
               fd_set rset;
               int    number_file;

               /* get udev event */
               FD_ZERO(&rset);
               FD_SET(fdin, &rset);

               //D_INFO("  select...\n");

               struct timeval timeout = { 0, 0 };

               while ((number_file = select(fdin+1, &rset, NULL, NULL, &timeout)) < 0) {
                    if (errno != EINTR) {
                         D_PERROR( "Insignia/Test:: select() failed!\n" );
                         return false;
                    }
               }

               if (FD_ISSET( fdin, &rset )) {
                    readPacket();
                    continue;
               }
          }

          break;
     }

     return false;
}

void
Test::readPacket()
{
     ssize_t ret;
     u32     length;

     //D_INFO("  read...\n");
     ret = read( fdin, &length, 4 );
     if (ret != 4)
          throw new Exception( "read(4) failed" );
     //D_INFO("    length %u...\n",length);


     Packet *p = (Packet *) new char[length];

     ret = read( fdin, &p->sender, length-4 );
     if (ret < 0) {
          D_PERROR( "Insignia::Test::readPacket: read(%u) failed!\n", length-4 );
          return;
     }
     else if ((size_t) ret != length-4)
          throw new Exception( Direct::String::F( "packet length mismatch (read %zd/%u)", ret, length-4 ) );

     //D_INFO("    done %zd...\n",ret);
     //D_INFO("    type %u...\n",p->type);
     switch (ret)
     {
          case -1:
               D_PERROR( "Test::Receive: read() failed!\n" );
               break;

          default:
               p->length = length;

               packets[p->type].push_back( p );

               if (p->type == Packet::TYPE_FINALISE) {
                    //fprintf( stderr, ">>>>>>>>>>>>>>>> received FINALISE from %u\n", p->sender );

                    D_ASSERT( num_slaves > 0 );
                    D_ASSERT( p->sender <= (s32) num_slaves );

                    if (!slave_exit[p->sender-1])
                         slave_exit[p->sender-1] = true;
               }

               packets_lwq.notifyAll();
               break;
     }
}

}

