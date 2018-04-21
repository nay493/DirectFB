#include <++dfb.h>

#include "InsigniaConfig.h"

/**********************************************************************************************************************/

namespace Insignia {


void
ConfigTemplateBase::GetConfigs( vector<Test::Config*> &configs )
{
     vector<Test::Config*> c = getConfigs();

     configs.insert( configs.end(), c.begin(), c.end() );
}

vector<Test::Config*>
ConfigTemplateBase::getConfigs( unsigned int option_index )
{
     vector<Test::Config*> result;

     if (option_index == options.size()) {
          Test::Config* config = newConfig();

          result.push_back( config );
     }
     else {
          ConfigOptionBase* option = options[option_index];

          for (size_t i=0; i<option->count(); i++) {
               vector<Test::Config*> c = getConfigs( option_index + 1 );

               for (vector<Test::Config*>::const_iterator it=c.begin(); it!=c.end(); it++) {
                    const ConfigTemplateBase* templ = dynamic_cast<const ConfigTemplateBase*>( *it );

//                    D_INFO( "option_index %u, option %zu/%zu\n", option_index, i, option->count() );

                    templ->options[option_index]->choose( i );

                    result.push_back( *it );
               }
          }
     }

     return result;
}


}

