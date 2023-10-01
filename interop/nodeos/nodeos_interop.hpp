#ifndef LEAP_NODEOS_INTEROP_H
#define LEAP_NODEOS_INTEROP_H

#include "deep_mind_interop_handler.hpp"

class NodeosInterop {
public:
    void Start(std::vector<std::string> args, interop_logger_base *interop_logger);
    int StartOnSeparateThread(std::vector<std::string> argv, interop_logger_base *interop_logger);
    static void StartExecutableFromLibrary();
//    void StartCompileMonitorServer();
//    void StartCompileMonitorClient();
};

#endif //LEAP_NODEOS_INTEROP_H
