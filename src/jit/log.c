#include "moar.h"

/* inline this? maybe */
void MVM_jit_log(MVMThreadContext *tc, const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (tc->instance->jit_log_fh) {
        vfprintf(tc->instance->jit_log_fh, fmt, args);
    }
    va_end(args);
}

static char * jitcode_name(MVMThreadContext *tc, MVMJitCode *code) {
    MVMuint64 cuuid_len;
    MVMuint64 name_len;
    MVMuint8 *cuuid = MVM_string_ascii_encode(tc, code->sf->body.cuuid,
                                              &cuuid_len);
    MVMuint8 *name  = MVM_string_ascii_encode(tc, code->sf->body.name,
                                              &name_len);
    MVMuint64 dirname_len = strlen(tc->instance->jit_bytecode_dir);
    // 3 chars for the separators, 4 for the postfix, 1 for the 0
    char *filename = malloc(dirname_len + name_len + cuuid_len + 8);
    memcpy(filename, tc->instance->jit_bytecode_dir, dirname_len);
    filename[dirname_len] = '/';
    memcpy(filename + dirname_len + 1, cuuid, cuuid_len);
    filename[dirname_len + cuuid_len + 1] = '.';
    memcpy(filename + dirname_len + cuuid_len + 2, name, name_len);
    memcpy(filename + dirname_len + cuuid_len + name_len + 2, ".bin", 5);
    free(name);
    free(cuuid);
    return filename;
}

void MVM_jit_log_bytecode(MVMThreadContext *tc, MVMJitCode *code) {
    char * filename = jitcode_name(tc, code);
    FILE * f = fopen(filename, "w");
    fwrite(code->func_ptr, sizeof(char), code->size, f);
    fclose(f);
    MVM_jit_log(tc, "Dump bytecode in %s\n", filename);
    free(filename);

}
