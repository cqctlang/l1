#include <signal.h>

#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "x/linenoise/linenoise.h"

void
l1__prompt(VM *vm, Imm argc, Val *argv, Val *rv)
{
    char *cprompt, *result;
    if (argc != 1)
        vmerr(vm, "wrong number of arguments to prompt");
    if (!Viskind(argv[0], Qstr))
        vmerr(vm, "operand 1 to prompt must be a string");
    cprompt = str2cstr(valstr(argv[0]));

    result = linenoise(cprompt);  /* TODO(natr): allow linenoise to use
                                     a custom allocator */
    efree(cprompt);

    if (result) {
        *rv = mkvalstr(mkstr0(result));
        linenoiseFree(result);
    } else {
        if (errno == EAGAIN) {
            /* ctrl-c */
            kill(getpid(), SIGINT);
        } else {
            /* actually could be oom, but treat as ctrl-d as there's
               presently no other way to detect this in linenoise */
            *rv = Xnil;
        }
    }
}

void fnrepl(Env env)
{
    FN(_prompt);
}
