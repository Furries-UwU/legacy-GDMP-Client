#pragma once
#include <functional>
#include <vector>
#include <mutex>

#include "layer/MultiplayerLayer.hpp"
#include "include.hpp"
#include "utility.hpp"
#include "global.hpp"

void executeInGDThread(std::function<void()>);