#include "graft_manager.h"

#include <misc_log_ex.h>
#include <boost/program_options.hpp>
#include <boost/property_tree/ini_parser.hpp>
// #include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>

namespace po = boost::program_options;
using namespace std;


int main(int argc, const char** argv)
{
    int log_level = 1;
    string config_filename;


    try {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help", "produce help message")
                ("config-file", po::value<string>(), "config filename (config.ini by default)")
                ("log-level", po::value<int>(), "log-level. (3 by default)");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }

        if (vm.count("config-file")) {
            config_filename = vm["config-file"].as<string>();
        }
        if (vm.count("log-level")) {
            log_level = vm["log-level"].as<int>();
        }
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }
    mlog_configure("", true);
    mlog_set_log_level(log_level);
    // load config
    boost::property_tree::ptree config;
    namespace fs = boost::filesystem;

    if (config_filename.empty()) {
        fs::path selfpath = argv[0];
        selfpath = selfpath.remove_filename();
        config_filename  = (selfpath /= "config.ini").string();
    }


    try {
        boost::property_tree::ini_parser::read_ini(config_filename, config);
        // now we have only 5 parameters
        // cryptonode
        // 1. rpc-address <IP>:<PORT>
        // 2. p2p-address <IP>:<PORT>
        // server
        // 3. address <IP>:<PORT>
        // 4. workers-count <integer>
        // 5. worker-queue-len <integer>
        const boost::property_tree::ptree& cryptonode_conf = config.get_child("cryptonode");
        const std::string cryptonode_rpc_address = cryptonode_conf.get<string>("rpc-address");
        const std::string cryptonode_p2p_address = cryptonode_conf.get<string>("p2p-address");

        graft::ServerOpts sopts;

        const boost::property_tree::ptree& server_conf = config.get_child("server");
        sopts.http_address = server_conf.get<string>("http-address");
        sopts.http_connection_timeout = server_conf.get<int>("http-connection-timeout");
        sopts.workers_count = server_conf.get<int>("workers-count");
        sopts.worker_queue_len = server_conf.get<int>("worker-queue-len");

        // TODO configure router
        graft::Router router;
        graft::Manager manager(router, sopts);
        graft::GraftServer server;

        // setup cryptonode connection params
        server.setCryptonodeP2PAddress(cryptonode_p2p_address);
        server.setCryptonodeRPCAddress(cryptonode_rpc_address);

        LOG_PRINT_L0("Starting server on " << server_address);
        server.serve(manager.get_mg_mgr());

    } catch (const std::exception & e) {
        std::cerr << "Exception thrown: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
