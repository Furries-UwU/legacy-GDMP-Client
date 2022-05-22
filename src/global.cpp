#include "global.hpp"

Global *Global::get()
{
    static auto global = new Global;
    return global;
}

void Global::queueInGDThread(const std::function<void()> &func)
{
    std::lock_guard<std::mutex> lock(this->gdThreadQueueMutex);
    this->gdThreadQueue.push_back(std::move(func));
}

void Global::executeGDThreadQueue()
{
    this->gdThreadQueueMutex.lock();
    auto buffer = std::move(this->gdThreadQueue);
    this->gdThreadQueueMutex.unlock();

    for (auto const &func : buffer)
    {
        func();

        this->gdThreadQueueMutex.lock();
        this->gdThreadQueue.erase(this->gdThreadQueue.begin());
        this->gdThreadQueueMutex.unlock();
    }
}