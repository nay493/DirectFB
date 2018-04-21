#include "InsigniaTest.h"

#include "InsigniaTestManager.h"


namespace Insignia {


TestManager::EntryMap TestManager::m_entries;


void
TestManager::RegisterTest( Test &test )
{
     Entry *entry = new Entry(test);

     std::pair<Direct::String,Entry*> pair(entry->info.name, entry);

     m_entries.insert( pair );

     direct_log_printf( NULL, "Register module '%s'\n", entry->info.name.c_str() );
}

}

