#include "global.hpp"

std::mutex globalMutex;

Global *Global::get()
{
    std::lock_guard<std::mutex> lock(globalMutex);
    static auto global = new Global;
    return global;
}