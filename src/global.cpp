#include "global.hpp"

void Global::queueInGDThread(const std::function<void()>& func)
{
    this->gdThreadQueue.push_back(func);
}

Global *Global::get()
{
    static auto global = new Global;
    return global;
}

void Global::executeGDThreadQueue()
{
    for (auto const &func : this->gdThreadQueue)
    {
        func();
        this->gdThreadQueue.erase(this->gdThreadQueue.begin());
    }
}