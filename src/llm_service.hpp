#pragma once

#include <string>
#include <sys/types.h>

#include <httplib.h>

#ifndef LLAMA_SERVER_PATH
#error "LLAMA_SERVER_PATH is not defined!"
#endif

class LlmService {
 public:
    LlmService();
    ~LlmService();

    std::string Complete(const std::string& prompt);

 private:
    pid_t pid_ {-1};
    httplib::Client client_;
};
