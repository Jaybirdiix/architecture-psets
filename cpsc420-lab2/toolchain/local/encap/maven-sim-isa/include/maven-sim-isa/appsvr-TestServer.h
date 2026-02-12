//========================================================================
// appsvr-TestServer.h
//========================================================================

#ifndef APPSVR_TEST_SERVER_H
#define APPSVR_TEST_SERVER_H

#include "htif.h"
#include "syscfg.h"
#include <map>

namespace appsvr {

  class TestServer {

   public:

    TestServer( htif_t& htif );
    ~TestServer();

    int run( int argc, char* argv[], char* envp[] );

   private:

    htif_t&  m_htif;
    uint32_t m_nprocs;

  };

}

#endif /* APPSVR_TEST_SERVER_H */

