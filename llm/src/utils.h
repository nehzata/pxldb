#ifndef LLM_UTILS_H
#define LLM_UTILS_H

#include <llama.h>

#include <string>
#include <vector>

void print_tokens(std::string name, std::vector<llama_token> &tokens);
#endif