#ifndef __INSIGNIA__TESTMANAGER_H__
#define __INSIGNIA__TESTMANAGER_H__

#include <map>

#include "InsigniaTest.h"


namespace Insignia {

     class TestManager {
     public:
          class Entry {
          public:
               Entry(Test &_test) : test(_test) {
                    test.GetInfo(info);
               }

               Test       &test;
               Test::Info  info;
          };

          typedef std::map<string,Entry*> EntryMap;

     public:
          static void RegisterTest( Test &test );

          static Test *LookupTest( const Direct::String &name ) {
               Entry *entry = m_entries[name];

               if (entry != NULL)
                    return &entry->test;

               return NULL;
          }

          static const EntryMap& GetEntries() {
               return m_entries;
          }

     private:
          static EntryMap m_entries;
     };

}

#endif

