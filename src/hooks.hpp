#pragma once
#include "include.hpp"
#include "utility.hpp"
#include "global.hpp"

void executeInGDThread(std::function<void()> f);