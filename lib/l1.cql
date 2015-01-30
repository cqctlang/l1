/*
    l1.cql

    This file contains funcitons for interacting with *another* l1 vm.
    Many of the functions are ported from l1 source code directly, though
    there some changes to account for #defines and incomplete types
    occassionally provided by sctl.
*/

// XXX document, give example usage, etc

@local regtos;

@export(
    //convenience functions not in l1
    codesrcstr,
    disasmstr,
    segstr,
    segstrs,
    btentry,
    srctostr,
    foreachframe,
    backtrace,
    //mkl1ns,
    randtostr,
    insntostr,
    valtostr,

    //the below are ported from l1

    //#defs from from syscqct.h
    cvaluenc,
    cvalu,
    cvalf,
    cvald,
    cldisp,
    //clsize,
    ra2size,
    ciddata,
    strdata,
    codeend,
    codeinsn,
    vecdata,
    //vecsize,
    vkind,

    //functions in code.c
    addr2line,

    //static function in code.c
    lookdbg,
    off2line,
    srcfileval,
    srclineval,
    srcfile,
    srcline,

    //functions in mem.c
    addr2seg //a2s
);

/* removed because of issues including syslib
@include <syslib.cqct>
@include <sctlns.cqct>
{
@local l1ns;
l1ns = nil;
@define mkl1ns(self...) {

    if(l1ns != nil) {
        return l1ns;
    }

    switch(length(self)) {
        case 0:
            switch(uname()[0]) {
                case "Linux":
                    self = "/proc/self/exe";
                    break;
                case "FreeBSD":
                case "Darwin":
                case "DragonFly":
                    self = "/proc/curproc/file";
                    break;
                case "NetBSD":
                    self = "/proc/curproc/exe";
                    break;
                default:
                    error("Cannot find base executable");
            }
            break;
        case 1:
            self = head(self);
            break;
        default:
            error("too many arguments");
    }

    l1ns = atnames(readsymlink(self));
    if(l1ns == nil) {
        error("could not make an ns for %s", self);
    }

    return l1ns;
}
l1ns = mkl1ns();
}
*/


@define cvaluenc(cv) { cv->v; }
@define cvalu(cv) { cv->v.u; }
@define cvalf(cv) { cv->v.f; }
@define cvald(cv) { cv->v.d; }

@define vecdata(x) { (Val*)((Vec*)x + 1); }
//XXX include local l1 dom?
//@define vecsize(n) { sizeof(Vec)+(n)*sizeof(Val); }

@define cldisp(c) { (Val*)((Closure*)c+1); }
//XXX include local l1 dom?
//@define clsize(n) {  sizeof(Closure)+(n)*sizeof(Val); }

@define vkind(p) {
    //return ((p->bits)>>Vkindoff) & Vkindmask;
    @local dom, t;
    //XXX for some reason, casting to Qkind doesn't work
    //when printed with printf("%e", (Qkind)v);
    //This hack fixes that.
    dom = domof(p);
    t = typedeftype(@typeof(dom`Qkind));
    return {t}(((Val)p)->bits >> 1) & 31;
}

@define codeend(code) {
    code = (Code*)code;
    return ((void*)code) + code->sz;
}
@define codeinsn(code) { (Insn*)(((Code*)code) + 1); }

@define off2line(code, off) {
    @local dat;
    dat = code->src;
    if (0 == dat) return nil;
    dat = vecdata(code->src);
    return (Src)dat[off];
}

@define ciddata(cid) {
    @local c;
    c = (char*)(cid+1);
    return stringof(c);
}

@define strdata(x) {
    return getbytes((char*)(x+1), x->len);
}

@define srcfileval(s) { vecdata(s)[0]; }
@define srclineval(s) { vecdata(s)[1]; }
@define srcfile(s) { ciddata((Cid*)(srcfileval(s))); }
@define srcline(s) { cvalu((Cval*)(srclineval(s))); }

@define lookdbg(c, off) {
    @local d, e, n;
    d = (Dbg*)strdata(c->dbg);
    if (d->off > off)
        error("Debug offset out of range");

    e = d+c->ndbg;
    while (d < e-1) {
        n = d+1;
        if (n->off > off)
            return d;
        d = n;
    }
    return d;
}

@define ra2size(ra, code) {
    @local off, d, dom;

    dom = domof(code);

    switch(code->kind) {
        case dom`Cnative:
        case dom`Cvm:
            if (ra >= codeend(code) || ra < (void*)codeinsn(code)) {
                @local seg;
                seg = addr2seg(ra);
                printf("%s\n", segstr(seg));
                error("bad return address: 0x%x 0x%x 0x%x",
                        ra, codeinsn(code), codeend(code));
            }
            off = ra-(uintptr)codeinsn(code);
            d = lookdbg(code, off);
            return d->fsz;
        case dom`Ccfn:
        case dom`Cccl:
            return (uintptr)ra;
        case dom`Calien:
        default:
            error("bad code kind: %e (%d)", code->kind, code->kind);
    }

    error("shouldn't get here: somebody did a bad thing to the above "
          "switch statement");
}

@defloc foreachstackframe(dom, base, pc, cl, fp, fn) {
    @local sz;

    fp = (Insn**)fp;

    //true before stack is initialized
    if (cl == 0) return;

    while (fp >= base) {
        sz = ra2size(pc, cl->code);

        fn(pc, cl, fp, sz);

        pc = fp[dom`Ora];
        cl = (Closure*)fp[dom`Ocl];

        //XXX really, this shouldn't happen
        if (0 == cl || dom`Qcl != vkind((Val)cl))
            return;

        fp -= sz;
    }
}

@define addr2line(c, pc...) {
    switch (length(pc)) {
        case 0:
            return off2line(c, 0);
        case 1:
            pc = head(pc);
            break;
        default:
            error("too many arguments");
    }
    return off2line(c, (Insn*)pc-(Insn*)codeinsn(c));
}

@define srctostr(src) {
    @local dom;
    dom = domof(src);
    if (srclineval(src) == dom`Xnil)
        return sprintfa("%s", srcfile(src));
    else
        return sprintfa("%s:%u", srcfile(src), srcline(src));
}

@define codesrcstr(c, pc...) {
    @local dom;

    switch (length(pc)) {
        case 0:
            pc = codeinsn(c);
            break;
        case 1:
            pc = head(pc);
            break;
        default:
            error("too many arguments");
    }

    dom = domof(c);

    switch(c->kind) {
    case dom`Ccfn:
        return "builtin function";
    case dom`Cccl:
        return "builtin closure";
    case dom`Cnative:
        return "native code";
    case dom`Calien:
        return "alien code";
    case dom`Cvm:
        return srctostr(addr2line(c, pc));
    }
    error("bad c->kind: %d\n", c->kind);

}

@define btentry(c, pc) {
    return sprintfa("%20s\t(%s)", ciddata(c->id), codesrcstr(c, pc));
}

@define foreachframe(vm, fn) {
    @local k, dom;

    dom = domof(vm);

    foreachstackframe(dom, vm->stk, vm->pc, vm->cl, vm->fp, fn);

    for(k=vm->klink; 0!=k && 0!=k->base; k=k->link) {
        fn(0, 0, 0, 0);
        foreachstackframe(dom, k->base, k->ra, k->cl, k->base+k->sz, fn);
    }
}

@define backtrace(vm, fd...) {
    switch(length(fd)) {
    case 0:
        fd = stdout;
        break;
    case 1:
        fd = head(fd);
        break;
    default:
        error("too many arguments");
    }

    foreachframe(vm, @lambda(pc, cl, fp, sz) {
        if (pc == 0) {
            fprintf(fd,"%20s\n", "-------------");
        } else {
            fprintf(fd,"%s\n", btentry(cl->code, pc));
        }
    });
}


@define segstr(seg) {
    @local d, t;
    d = domof(seg);
    t = subtype(@typeof(d`MThole));
    sprintfa("%e %d 0x%x-0x%x %d",
            {t}seg->mt, seg->gen, seg->a, seg->e, seg->n);
}

@define segstrs(l1dom) {
    @local strs, mx, i, isfree;

    strs = [];
    //mx = (l1dom`segmap.hi-l1dom`segmap.lo)/l1dom`Segsize;
    mx = (l1dom`segmap.hi-l1dom`segmap.lo)/((uintptr){l1dom}1<<20);

    isfree = 0;
    for(i=0; i<mx; ++i) {
        if (!isfree)
            append(strs, sprintfa("%d %s", i, segstr(&l1dom`segmap.map[i])));
        isfree =
            l1dom`segmap.map[i].mt == l1dom`MTfree ||
            l1dom`segmap.map[i].mt == l1dom`MThole;
    }
    append(strs, sprintfa("%d %s", i, segstr(&l1dom`segmap.map[i])));

    return join(strs, "\n");
}

//a2s in l1
@define addr2seg(a) {
    @local dom, o;
    dom = domof(a);
    //o = ((void*)a-segmap.lo)/Segsize;
    o = ((void*)a-dom`segmap.lo)/((uintptr){dom}1<<20);
    return dom`segmap.map+o;
}

@defloc insscnt(i) {
    return *(int64*)((uintptr)i + 0x40);
}

@define insntostr(i) {
    @local dom, t;
    dom = domof(i);
    t = subtype(@typeof(dom`Icall));
    switch(i->kind) {
        case dom`Iapply:
        case dom`Ichkint:
        case dom`Iret:
        case dom`Iiret:
        case dom`Ihalt:
        case dom`Igcpoll:
        case dom`Iunderflow:
        case dom`Ikg:
        case dom`Ikp:
        case dom`Inop:
        case dom`Iabort:
            return sprintfa("%e", {t}i->kind);
        case dom`Ijnz:
        case dom`Ijz:
            //XXX print out jmp target as in Ijmp below
            //fallthrough
        case dom`Iargc:
        case dom`Ivargc:
        case dom`Ichkcl:
        case dom`Ichksp:
        case dom`Imovvc:
        case dom`Icall:
        case dom`Icallt:
        case dom`Ibox:
        case dom`Ibox0:
            return sprintfa("%e %s", {t}i->kind, randtostr(&i->op1));
        case dom`Iaddfp:
            return sprintfa("%e %d", {t}i->kind, insscnt(i));
        case dom`Ijmp:
            return sprintfa("%e %d (0x%x)", {t}i->kind, insscnt(i),
                            i+insscnt(i));
        case dom`Imov:
        case dom`Inot:
        case dom`Iinv:
        case dom`Ineg:
            return sprintfa("%e %s -> %s", {t}i->kind, randtostr(&i->op1),
                            randtostr(&i->dst));
        default:
            return sprintfa("%e %s %s -> %s", {t}i->kind, randtostr(&i->op1),
                            randtostr(&i->op2), randtostr(&i->dst));
    }

}

regtos = [
    "%vc",
    "%fp",
    "%pc",
    "%ac",
    "%cl"
];

@defloc OIDX(m) { return m>>4; }
@defloc OBOX(m) { return (m>>3) & 1; }
@defloc OKIND(m) { return m & 7; }

@defloc randval(r) { return *(Val*)((Val*)r + 1); }
@defloc randimm(r) { return *(Imm*)((Val*)r + 1); }

@define randtostr(r) {
    @local dom, md, t;
    dom = domof(r);
    md = OKIND(r->mode);
    switch(md) {
        case dom`Oreg:
            return regtos[md];
        case dom`Oframe:
            t = sprintfa("%d(%s)", OIDX(r->mode), regtos[dom`Rfp]);
            if(OBOX(r->mode)) return sprintfa("[%s]", t);
            else return t;
        case dom`Odisp:
            t = sprintfa("%d(%s)", OIDX(r->mode), regtos[dom`Rcl]);
            if(OBOX(r->mode)) return sprintfa("[%s]", t);
            else return t;
        case dom`Otopl:
            return sprintfa("<%s>", ciddata((Cid*)r->val));
        case dom`Oimm:
            return sprintfa("%lu", randimm(r));
        case dom`Otopr:
            t = (Pair*)randval(r);
            return sprintfa("cons(<%s>,_)", ciddata((Cid*)t->car));
        case dom`Oval:
            return valtostr(randval(r));
        case dom`Onil:
            return "nil";
    }
    error("Unknown operand kind %d", md);
}

@define valtostr(v) {
    @local dom;
    dom = domof(v);
    switch (vkind(v)) {
    case dom`Qcval:
        return sprintfa("%llu", cvalu((Cval*)v));
    case dom`Qcid:
        return sprintfa("'%s", ciddata((Cid*)v));
    case dom`Qstr:
        return sprintfa("\"%s\"", strdata((Str*)v));
    case dom`Qctype:
        //XXX
        //fallthrough
    default:
        return sprintfa("<%e %p>", vkind(v), v);
    }
}

@define disasmstr(code) {
    @local i, r, e;
    r = [];
    e = codeend(code);
    for(i=codeinsn(code); i<e; ++i) {
        append(r, sprintfa("    0x%x %s", i, insntostr(i)));
    }
    return join(r, "\n");
}
