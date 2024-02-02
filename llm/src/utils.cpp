#include "utils.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

void print_tokens(std::string name, std::vector<llama_token> &tokens) {
    std::string tmp;
    for (auto &t : tokens) {
        tmp += fmt::format("{:04x}:", t);
    }
    spdlog::debug("{}: {}", name, tmp);
}