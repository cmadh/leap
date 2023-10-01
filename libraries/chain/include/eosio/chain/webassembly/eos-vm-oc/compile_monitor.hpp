#pragma once

#include <eosio/chain/webassembly/eos-vm-oc/config.hpp>

#include <boost/asio/local/datagram_protocol.hpp>
#include <eosio/chain/webassembly/eos-vm-oc/ipc_helpers.hpp>

namespace eosio { namespace chain { namespace eosvmoc {

wrapped_fd get_connection_to_compile_monitor(int cache_fd);

//#ifdef ENABLE_OC_CLIENT_SERVER_COMPILE_MONITOR_TRAMPOLINE

extern "C" __attribute__((visibility("default"))) void start_compile_monitor_trampoline_server(const char* SOCKET_PATH);

extern "C" __attribute__((visibility("default"))) void start_compile_monitor_trampoline_client(const char* SOCKET_PATH);

//#endif

}}}