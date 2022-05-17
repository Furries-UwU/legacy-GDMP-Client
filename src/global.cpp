#include "global.hpp"

void Global::queueInGDThread(const std::function<void()>& func)
{
    this->gdThreadQueueMutex.lock();

    this->gdThreadQueue.push_back(func);

    this->gdThreadQueueMutex.unlock();
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
        this->gdThreadQueueMutex.lock();

        func();
        this->gdThreadQueue.erase(this->gdThreadQueue.begin());

        this->gdThreadQueueMutex.unlock();
    }
}