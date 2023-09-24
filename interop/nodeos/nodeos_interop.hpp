#ifndef LEAP_NODEOS_INTEROP_H
#define LEAP_NODEOS_INTEROP_H

#include "deep_mind_interop_handler.hpp"

class NodeosInterop {
public:
    void StartNodeos(std::vector<std::string> args, interop_logger_base *interop_logger);
    int Start(std::vector<std::string> argv, interop_logger_base *interop_logger);
};

#endif //LEAP_NODEOS_INTEROP_H
