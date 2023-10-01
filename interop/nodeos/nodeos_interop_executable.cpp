#include <eosio/chain/webassembly/eos-vm-oc/compile_monitor.hpp>
#include <boost/exception/diagnostic_information.hpp>

extern "C" __attribute__((visibility("default"))) int main(int argc, char** argv)
{
    std::cout << "main called" << std::endl;
    try {
        std::cout << "MAIN" << std::endl;
        eosio::chain::eosvmoc::start_compile_monitor_trampoline_server("/tmp/nodeos_compile_monitor_socket");
    } catch( const boost::exception& e ) {
        std::cout << boost::diagnostic_information(e) << std::endl;
        return 1;
    } catch( const std::runtime_error& e ) {
        std::cout << e.what() << std::endl;
        return 3;
    } catch( const std::exception& e ) {
        std::cout << e.what() << std::endl;
        return 4;
    } catch( ... ) {
        std::cout << "unknown exception" << std::endl;
        return 5;
    }
    std::cout << "main END" << std::endl;
    return 0;
}