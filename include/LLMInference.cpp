#include "LLMInference.h"
#include <cstring>
#include <stdexcept>
#include <vector>
#include <string>
#include <cstdio>

LLMInference::LLMInference() {}

LLMInference::~LLMInference() {
    if (_ctx) llama_free(_ctx);
    if (_model) llama_free_model(_model);
}

bool LLMInference::loadModel(const std::string& model_path) {
    if (_ctx)   llama_free(_ctx);
    if (_model) llama_free_model(_model);

    llama_model_params model_params = llama_model_default_params();
    _model = llama_load_model_from_file(model_path.c_str(), model_params);
    if (!_model) {
        fprintf(stderr, "Falha ao carregar o modelo\n");
        return false;
    }

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = 512;
    _ctx = llama_init_from_model(_model, ctx_params);
    if (!_ctx) {
        fprintf(stderr, "Falha ao criar o contexto\n");
        llama_free_model(_model);
        _model = nullptr;
        return false;
    }

    return true;
}

std::string LLMInference::predict(const std::string& prompt, int max_tokens) {
    if (!_model || !_ctx) {
        return "[ERRO: Modelo não carregado]";
    }

    std::vector<llama_token> tokens(prompt.size() + 8);
    int n_tokens = llama_tokenize((const llama_vocab*)_model, prompt.c_str(), prompt.length(),
                                  tokens.data(), tokens.size(), true, false);
    if (n_tokens < 0) {
        tokens.resize(-n_tokens);
        n_tokens = llama_tokenize((const llama_vocab*)_model, prompt.c_str(), prompt.length(),
                                  tokens.data(), tokens.size(), true, false);
    }
    tokens.resize(n_tokens);

    // "Eval" inicial
    llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());
    llama_decode(_ctx, batch);


    std::string result = "";

    for (int i = 0; i < max_tokens; ++i) {
        const float* logits = llama_get_logits(_ctx);

        // greedy sampling manual: pegar o token com maior logit
        llama_token best_token = 0;
        float best_logit = logits[0];
        for (llama_token token_id = 1; token_id < llama_n_vocab((const llama_vocab*)_model); ++token_id) {
            if (logits[token_id] > best_logit) {
                best_logit = logits[token_id];
                best_token = token_id;
            }
        }

        if (best_token == llama_token_eos((const llama_vocab*)_model)) {
            break;
        }

        // converter token em string
        char buffer[512];
        llama_token_to_piece((const llama_vocab*)_model, best_token, buffer, sizeof(buffer), 0, false);
        result += buffer;

        // // passar o novo token para o modelo
        // if (llama_eval(_ctx, &best_token, 1, tokens.size() + i, 1)) {
        //     fprintf(stderr, "Erro no llama_eval durante geração\n");
        //     break;
        // }
    }

    return result;
}
