#ifndef LLMINFERENCE_H
#define LLMINFERENCE_H

#include "llama.h"
#include "common.h"
#include <string>
#include <vector>

class LLMInference {
private:
    llama_model* _model = nullptr;
    llama_context* _ctx = nullptr;

public:
    LLMInference();
    ~LLMInference();

    bool loadModel(const std::string& modelPath);
    std::string predict(const std::string& prompt, int max_tokens);
};

#endif
