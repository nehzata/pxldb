#ifndef APP_LLM_H
#define APP_LLM_H

#ifdef __cplusplus
extern "C" {
#endif

int llm_init(const char *fp);
void llm_run();

void llm_set_schema(int schema_id, const char *schema);

struct llm_ctx {
    void *app;
    char *event_id;
    int req_id;
    int error;
    char *error_code;
    int cancel;
    int stop;
    char *token;
};
void llm_prime(int schema_id, const char *schema, struct llm_ctx *r);

typedef void (*llm_autocomplete_callback_fn)(struct llm_ctx *r);
int llm_autocomplete(int schema_id, const char *schema, llm_autocomplete_callback_fn cb, struct llm_ctx *r);

void llm_autocomplete_callback(struct llm_ctx *r);

#ifdef __cplusplus
}
#endif

#endif