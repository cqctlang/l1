#include <stdlib.h>
#include <string.h>

#include "cqct.h"
#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "vm-options.h"

static void free_loadpath(struct vm_options* options) {
    char* entry;
    for (entry = *options->loadpath; entry != 0; ++entry) {
        free(entry);
    }
    free(options->loadpath);
    options->loadpath = 0;
}

void cqct_options_init(struct vm_options* options) {
    memset(options, 0, sizeof(*options));
}

void cqct_options_set_memfile(struct vm_options* options, const char* memfile) {
    if (options->memfile) {
        free(options->memfile);
    }
    options->memfile = strdup(memfile);
}

void cqct_options_set_loadpath(struct vm_options* options,
                               const char** loadpath) {
    const char* entry;
    char **cur;
    int nentries = 0;
    if (options->loadpath) {
        free_loadpath(options);
    }

    for (entry = *loadpath; entry != 0; ++entry) {
        ++nentries;
    }

    options->loadpath = (char**)malloc((nentries + 1) * sizeof(char*));
    cur = options->loadpath;
    for (entry = *loadpath; entry != 0; ++entry) {
        *(cur++) = strdup(entry);
    }
    *cur = 0;
}
