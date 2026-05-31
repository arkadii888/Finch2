#pragma once

#include <string>
#include <sys/types.h>

#include <httplib.h>

#ifndef LLAMA_SERVER_PATH
#error "LLAMA_SERVER_PATH is not defined!"
#endif

class LlmService {
 public:
    LlmService(const std::string& model_path, int port = 8889);
    ~LlmService();

    std::string Complete(const std::string& prompt);

 private:
    pid_t pid_ = -1;
    httplib::Client client_;
    std::string model_path_;
};
