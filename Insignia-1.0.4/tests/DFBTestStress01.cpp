#include <++dfb.h>

#include <InsigniaTest.h>


class DFBTestStress01 : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestStress01 );

private:
     IDirectFB m_dfb;

public:
     class ConfigStress01 : public Config {
     public:
          int keep;

          ConfigStress01( unsigned int num_processes,
                          int          keep )
               :
               keep( keep )
          {
               this->num_processes = num_processes;
          }

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );

               string.PrintF( "  || Keep: %d", keep );
          }
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          int keeps[] = {
               0,
               1,
               2,
               5,
               10,
               20,
               50,
               100,
               200,
               500,
               -1
          };

          for (unsigned int np=1; np<=32; np++) {
               for (unsigned int k=0; k<D_ARRAY_SIZE(keeps); k++)
                    configs.push_back( new ConfigStress01( np, keeps[k] ) );
          }
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestStress01";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual Result Run( const Config& configuration ) {
          const ConfigStress01& config = dynamic_cast<const ConfigStress01&>(configuration);

          if (config.num_processes > 1 && setup.process_index == 0) {
               receiveSlavesExit();
          }
          else {
               IDirectFBSurface              surfaces[config.keep+1];
               std::vector<IDirectFBSurface> all;
               int                           n = 0;

               while (!stopped()) {
                    DFBSurfaceDescription desc;

                    desc.flags  = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT);
                    desc.width  = 100;
                    desc.height = 100;

                    switch (config.keep) {
                         case -1:
                              try {
                                   all.push_back( m_dfb.CreateSurface( desc ) );
                              }
                              catch (DFBException *ex) {
                                   if (!strcmp( ex->GetAction(), "CreateSurface" )) {
                                        postMasterExit();
                                        return RESULT_SUCCESS;
                                   }

                                   throw ex;
                              }
                              break;

                         case 0:
                              m_dfb.CreateSurface( desc );
                              break;

                         default:
                              surfaces[n++ % config.keep] = m_dfb.CreateSurface( desc );
                              break;
                    }
               }

               postMasterExit();
          }

          return RESULT_SUCCESS;
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }
};


INSIGNIA_TEST_STATICS( DFBTestStress01 );

