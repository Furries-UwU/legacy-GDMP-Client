#include "global.hpp"

Global* Global::get() {
    static auto global = new Global;
    return global;
}