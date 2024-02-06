#include "llm.h"

#include <model.h>
#include <pthread.h>
#include <stdio.h>

#include <string>

model *m = NULL;
bool shutdown = false;
pthread_t id;

int llm_init(const char *fp) {
    if (m != NULL) {
        return 1;
    }

    shutdown = false;
    m = new model(fp, 0, 512, 8);
    return m->init();
}

void llm_close() {
    shutdown = true;
    pthread_join(id, NULL);
    delete m;
    m = NULL;
}

void *_run(void *params) {
    while (!shutdown) {
        m->run();
    }
    return NULL;
}

void llm_run() { pthread_create(&id, NULL, _run, NULL); }

void llm_set_schema(int schema_id, const char *schema) {
    std::string s(schema);
    m->set_schema(schema_id, s);
}

void llm_prime(int schema_id, const char *prompt, struct llm_ctx *r) {
    std::string p(prompt);
    unsigned int task_id = m->prime(schema_id, p);
    if (task_id == 0) {
        r->error = true;
        snprintf(r->error_code, sizeof(r->error_code), "ERROR_INVALID_REQUEST");
        return;
    }

    task_result res;
    bool found = m->next_result(task_id, res);
    if (!found) {
        r->error = true;
        snprintf(r->error_code, sizeof(r->error_code), "ERROR_TIMEOUT");
    } else if (res.error) {
        r->error = true;
        snprintf(r->error_code, sizeof(r->error_code), "ERROR_UNKNOWN");
    } else if (res.cancel) {
        r->cancel = true;
    }
}

int llm_autocomplete(int schema_id, const char *prompt, llm_autocomplete_callback_fn cb, struct llm_ctx *r) {
    std::string p(prompt);
    unsigned int task_id = m->autocomplete(schema_id, p);
    if (task_id == 0) {
        return 1;
    }

    while (true) {
        task_result res;
        bool found = m->next_result(task_id, res);
        if (!found) {
            r->error = true;
            snprintf(r->error_code, sizeof(r->error_code), "ERROR_TIMEOUT");
            cb(r);
            break;
        } else if (res.error) {
            r->error = true;
            snprintf(r->error_code, sizeof(r->error_code), "ERROR_UNKNOWN");
            cb(r);
            break;
        } else if (res.cancel) {
            r->cancel = true;
            cb(r);
            break;
        } else if (res.stop) {
            r->stop = true;
            cb(r);
            break;
        }

        snprintf(r->token, sizeof(r->token), "%s", res.t.c_str());
        cb(r);
    }

    spdlog::info("llm_autocomplete: done");

    return 0;
}

extern "C" void go_llm_autocomplete_callback(void *r);
void llm_autocomplete_callback(struct llm_ctx *r) { go_llm_autocomplete_callback(r); }