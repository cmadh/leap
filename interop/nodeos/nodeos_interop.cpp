#include <eosio/chain/application.hpp>

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>
#include <eosio/net_plugin/net_plugin.hpp>
#include <eosio/producer_plugin/producer_plugin.hpp>
#include <eosio/resource_monitor_plugin/resource_monitor_plugin.hpp>
#include <eosio/version/version.hpp>

#include <fc/log/logger.hpp>
#include <fc/log/logger_config.hpp>
#include <fc/log/appender.hpp>
#include <fc/exception/exception.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include "config.hpp"
#include "nodeos_interop.hpp"

#include <thread>
#include <future>

using namespace appbase;
using namespace eosio;

eosio::chain::deep_mind_interop_handler _deep_mind_interop_log = deep_mind_interop_handler();


namespace detail {

    fc::logging_config& add_deep_mind_logger(fc::logging_config& config) {
        config.appenders.push_back(
                fc::appender_config( "deep-mind", "dmlog" )
        );

        fc::logger_config dmlc;
        dmlc.name = "deep-mind";
        dmlc.level = fc::log_level::debug;
        dmlc.enabled = true;
        dmlc.appenders.push_back("deep-mind");

        config.loggers.push_back( dmlc );

        return config;
    }

    void configure_logging(const std::filesystem::path& config_path)
    {
        try {
            try {
                if( std::filesystem::exists( config_path ) ) {
                    fc::configure_logging( config_path );
                } else {
                    auto cfg = fc::logging_config::default_config();

                    fc::configure_logging( ::detail::add_deep_mind_logger(cfg) );
                }
            } catch (...) {
                elog("Error reloading logging.json");
                throw;
            }
        } catch (const fc::exception& e) {
            elog("${e}", ("e",e.to_detail_string()));
        } catch (const boost::exception& e) {
            elog("${e}", ("e",boost::diagnostic_information(e)));
        } catch (const std::exception& e) {
            elog("${e}", ("e",e.what()));
        } catch (...) {
            // empty
        }
    }

} // namespace detail

void logging_conf_handler()
{
    auto config_path = app().get_logging_conf();
    if( std::filesystem::exists( config_path ) ) {
        ilog( "Received HUP.  Reloading logging configuration from ${p}.", ("p", config_path.string()) );
    } else {
        ilog( "Received HUP.  No log config found at ${p}, setting to default.", ("p", config_path.string()) );
    }
    ::detail::configure_logging( config_path );
    fc::log_config::initialize_appenders();
}

void initialize_logging()
{
    auto config_path = app().get_logging_conf();
    if(std::filesystem::exists(config_path))
        fc::configure_logging(config_path); // intentionally allowing exceptions to escape
    else {
        auto cfg = fc::logging_config::default_config();

        fc::configure_logging( ::detail::add_deep_mind_logger(cfg) );
    }

    fc::log_config::initialize_appenders();

    app().set_sighup_callback(logging_conf_handler);
}

enum return_codes {
    OTHER_FAIL        = -2,
    INITIALIZE_FAIL   = -1,
    SUCCESS           = 0,
    BAD_ALLOC         = 1,
    DATABASE_DIRTY    = 2,
    FIXED_REVERSIBLE  = SUCCESS,
    EXTRACTED_GENESIS = SUCCESS,
    NODE_MANAGEMENT_SUCCESS = 5
};

void NodeosInterop::StartNodeos(std::vector<std::string> args, interop_logger_base *interop_logger){
    try {
        pthread_setname_np(pthread_self(), "nodeos-thread");
        ilog("ARGS:");
        for(auto arg_it = args.begin(); arg_it != args.end(); arg_it++){
            ilog(*arg_it);
        }

        std::vector<char*> cstrings;
        cstrings.reserve(args.size());

        for(auto& s: args)
            cstrings.push_back(&s[0]);

        _deep_mind_interop_log._interop_logger = interop_logger;

        app().set_version(eosio::nodeos::config::version);
        app().set_version_string(eosio::version::version_client());
        app().set_full_version_string(eosio::version::version_full());

        auto root = fc::app_path();
        ilog(root.string());
        ilog(root.generic_string());
        app().set_default_data_dir(root / "eosio" / nodeos::config::node_executable_name / "data" );
        app().set_default_config_dir(root / "eosio" / nodeos::config::node_executable_name / "config" );
        http_plugin::set_defaults({
                                          .default_unix_socket_path = "",
                                          .default_http_port = 8888
                                  });
        if(!app().initialize<chain_plugin, net_plugin, producer_plugin, resource_monitor_plugin>(cstrings.size(), cstrings.data())) {
            const auto& opts = app().get_options();
            if( opts.count("help") || opts.count("version") || opts.count("full-version") || opts.count("print-default-config") ) {
                // SUCCESS;
            }
            // INITIALIZE_FAIL;
        }
        if (auto resmon_plugin = app().find_plugin<resource_monitor_plugin>()) {
            resmon_plugin->monitor_directory(app().data_dir());
        } else {
            elog("resource_monitor_plugin failed to initialize");
            // INITIALIZE_FAIL;
        }
        if (auto chain_plug = app().find_plugin<chain_plugin>()) {
            chain_plug->chain().enable_deep_mind(&_deep_mind_interop_log);
            ilog("enabled interop deep-mind");
        } else {
            elog("failed to enable interop deep_mind");
            // INITIALIZE_FAIL;
        }
        initialize_logging();
        ilog( "${name} version ${ver} ${fv}",
              ("name", nodeos::config::node_executable_name)("ver", app().version_string())
                      ("fv", app().version_string() == app().full_version_string() ? "" : app().full_version_string()) );
        ilog("${name} using configuration file ${c}", ("name", nodeos::config::node_executable_name)("c", app().full_config_file_path().string()));
        ilog("${name} data directory is ${d}", ("name", nodeos::config::node_executable_name)("d", app().data_dir().string()));
        app().startup();
        app().set_thread_priority_max();
        app().exec();
    } catch( const extract_genesis_state_exception& e ) {
        // EXTRACTED_GENESIS;
    } catch( const fixed_reversible_db_exception& e ) {
        // FIXED_REVERSIBLE;
    } catch( const node_management_success& e ) {
        // NODE_MANAGEMENT_SUCCESS;
    } catch( const fc::exception& e ) {
        if( e.code() == fc::std_exception_code ) {
            if( e.top_message().find( "atabase dirty flag set" ) != std::string::npos ) {
                elog( "database dirty flag set (likely due to unclean shutdown): replay required" );
                // DATABASE_DIRTY;
            }
        }
        elog( "${e}", ("e", e.to_detail_string()));
        // OTHER_FAIL;
    } catch( const boost::interprocess::bad_alloc& e ) {
        elog("bad alloc");
        // BAD_ALLOC;
    } catch( const boost::exception& e ) {
        elog("${e}", ("e",boost::diagnostic_information(e)));
        // OTHER_FAIL;
    } catch( const std::runtime_error& e ) {
        if( std::string(e.what()).find("atabase dirty flag set") != std::string::npos ) {
            elog( "database dirty flag set (likely due to unclean shutdown): replay required" );
            // DATABASE_DIRTY;
        } else {
            elog( "${e}", ("e",e.what()));
        }
        // OTHER_FAIL;
    } catch( const std::exception& e ) {
        elog("${e}", ("e",e.what()));
        // OTHER_FAIL;
    } catch( ... ) {
        elog("unknown exception");
        // OTHER_FAIL;
    }
}

int NodeosInterop::Start(std::vector<std::string> args, interop_logger_base *interop_logger)
{
    std::thread thread_object(&NodeosInterop::StartNodeos, this, args, interop_logger);
//    int i = ret.get();
//    if(i != 0)
//        return i;
//    std::thread thread_obj(StartNodeos, argc, args, interop_logger);
    thread_object.join();
    ilog("${name} successfully exiting", ("name", nodeos::config::node_executable_name));
    return SUCCESS;
}
