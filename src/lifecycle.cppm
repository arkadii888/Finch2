module;

#include <mutex>
#include <atomic>
#include <csignal>

export module lifecycle;

std::atomic<bool> is_alive {true};

void SignalHandler(int signal) {
    is_alive = false;
}

std::once_flag signals_flag;

export namespace lifecycle {
    void Init() {
        std::call_once(signals_flag, [](){
            std::signal(SIGINT, SignalHandler);
            std::signal(SIGTERM, SignalHandler);
        });
    }

    const std::atomic<bool>& is_alive_public {is_alive};
}
