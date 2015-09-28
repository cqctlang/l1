@local max_indent, max_string;

//if you're looking here for more information about this library, you may
//also want to look at the test code, which provides example usage.
//see <l1>/test/memstr.l1

//XXX there are still some newline issues that should be worked out. The
//logic for determining when to end a line is not clean enough.

@export( cptrstr, setmaxindent, setmaxstring );

@defloc cvalstr(t, v) {
    if (isenum(t) || isenumconst(t)) {
        return sprintfa("%e (%d)", {t}v, v);
    }
    switch(basebase(t)) {
    case cqct`Vbool:
        return sprintfa("%a", v);
    case cqct`Vchar:
    case cqct`Vuchar:
        return sprintfa("%d '%c' (0x%x)", v, v, v);
    case cqct`Vshort:
    case cqct`Vint:
    case cqct`Vlong:
    case cqct`Vvlong:
    case cqct`Vushort:
    case cqct`Vuint:
    case cqct`Vulong:
    case cqct`Vuvlong:
        return sprintfa("%lld (0x%llx)", v, v);
    default:
        return sprintfa("%a", v);
    }
}

max_indent = 60;
max_string = 40;

/*
Sets the maximum amount of indentation that will be allowed. -1 for infinite.
*/
@define setmaxindent(x) {
    @local t;

    if (!iscvalue(x)) error("must provide an integer");
    t = @typeof(x);
    if (!isbase(t) || basebase(t) != cqct`Vint)
        error("must provide an integer");
    max_indent = x;
}

/*
Sets the maximum length of a string that will be printed.
For no limit, use -1
*/
@define setmaxstring(x) {
    @local t;

    if (!iscvalue(x)) error("must provide an integer");
    t = @typeof(x);
    if (!isbase(t) || basebase(t) != cqct`Vint)
        error("must provide an integer");
    max_string = x;
}

/**
* returns a string representation of the object pointed to by x (including
* its values in memory).
* params: x is a pointer to an object
* depth: number of recursive calls to make -- -1 for unlimited
* handlers: handlers table
* returns: a string representation of x and its content
*
* The handlers table maps ctypes to functions that produce output for that
* ctype. The handles are called with the signature:
*   handler(p, i, cps)
* where p is a pointer to an object of the associated ctype, i is the current
* indentation string, and cps is a recursion-free instance of cptrstr. The
* hander is expected to return an un-indented string representation of p.
* The handler may use cps to get a string representation of memory -- calls
* to cps will *not* use the given handler for its type but will use all other
* handlers provided to the original call to cptrstr. Any newlines in the
* returned string should be followed by the indent string i -- this excepts any
* newlines from the output of cps, which handles indentation correctly.
*
* depth is -1 by default. handlers is [:] (the empty table) by default.
* max_indent takes priority over the depth parameter.
*/
@define cptrstr(x, params...)
{
    @local dom, handlers, depth;

    if (length(params)>2) {
        error("too many parameters");
    }

    handlers = depth = nil;
    foreach(@lambda(p) {
        if (istable(p)) {
            if (handlers != nil) {
                error("too many handler tables provided");
            }
            handlers = p;
        }
        if (iscvalue(p)) {
            @local t;
            t = @typeof(p);
            if (!isbase(t) || basebase(t) != cqct`Vint) {
                error("depth must be an integer");
            }
            if (depth != nil) {
                error("too many depths provided");
            }
            depth = p;
        }
    }, params);
    if (handlers == nil) {
        handlers = [:];
    }
    if (depth == nil) {
        depth = -1;
    }

    dom = domof(x);

    @defloc handler_char(p, i, cs) {
        @local str;
        if (!ismapped(p)) {
            str = "UNMAPPED";
        } else {
            str = stringof(p);
        }
        if (max_string != -1 && length(str) > max_string) {
            str = substr(str, 0, max_string) + "...";
        }
        return sprintfa("@0x%x %t = %a", p, @typeof(p), str);
    }

    //default handlers
    if (handlers[@typeof(dom`char)] == nil) {
        handlers[@typeof(dom`char)] = handler_char;
    }

    @defloc handler_void(p, i, cs) {
        return sprintfa("@0x%x %t", p, @typeof(p));
    }
    if (handlers[@typeof(dom`void)] == nil) {
        handlers[@typeof(dom`void)] = handler_void;
    }

    //to prevent infinite recursion when a handler simply
    //wants to ammend the default output for cptrstr (with all of
    //the other handlers intact), we always call any type handler
    //through this function.
    @defloc dohandler(t, p, d, indent) {
        @local fn;
        fn = handlers[t];
        return dynamicwind(
                @lambda() { delete(handlers,t); },
                @lambda() { fn(p, indent,
                               @lambda(p) { cptrstr_rec(p, d, indent); }); },
                @lambda() { handlers[t] = fn; }
              );
    }

    @defloc cptrstr_rec(p, depth, indent) {
        @local t;

        //indent is the amount to indent all except the first line
        //(the first line will be appropriately indented by the caller)

        //XXX what if the type t is not defined in the ns
        t = @typeof(p);

        if (handlers[t] != nil) {
            return dohandler(t, p, depth, indent);
        }

        while(istypedef(t)) {
            t = typedeftype(t);
            if (handlers[t] != nil) {
                return dohandler(t, p, depth, indent);
            }
        }

        if (isptr(t)) {
            @local t2, newdepth;

            t2 = subtype(t);

            if (handlers[t2] != nil) {
                return dohandler(t2, p, depth, indent);
            }

            while(istypedef(t2)) {
                t2 = typedeftype(t2);
                if (handlers[t2] != nil) {
                    return dohandler(t2, p, depth, indent);
                }
            }

            //first, handle all types without a recursive call
            if (isvoid(t2)) {
                return sprintfa("@0x%x void*", p);
            }
            if (isfunc(t2)) {
                return sprintfa("@0x%x <code>", p);
            }
            if (isbitfield(t2)) {
                @local str;
                if (!ismapped((char*)p, sizeof(bitfieldcontainer(t2)))) {
                    str = "UNMAPPED";
                } else {
                    str = sprintfa("%lld [0x%llx]", *p, *p);
                }
                return sprintfa("@0x%x bitfield:[0x%x:0x%x] = %s",
                                p, bitfieldpos(t2), bitfieldwidth(t2), str);
            }
            if (isundeftype(t2)) {
                return sprintfa("@0x%x %t <UNDEFINED TYPE>", p, t2);
            }
            if (isbase(t2)) {
                return sprintfa("@0x%x %t = %s", p, t2, cvalstr(t2,*p));
            }

            if (depth == 0) {
                return sprintfa("@0x%x %t ...", p, t2);
            }
            if (depth == -1) {
                newdepth = -1;
            } else {
                newdepth = depth - 1;
            }

            if (max_indent != -1 && length(indent) >= max_indent) {
                return sprintfa("@0x%x %t ...", p, t2);
            }

            //now, do all types with a recursive call
            if (issu(t2)) {
                @local flds, i, nxt_indent, ret, fldstrs;
                @local mxoff, mxtype, mxid;

                if (!ismapped(p)) return "UNMAPPED";

                ret = [];
                flds = fields(t2);

                append(ret, sprintfa("@0x%x ", p));

                if (isstruct(t2)) append(ret, "struct");
                else if (isunion(t2)) append(ret, "union");
                else append(ret, "aggregate"); //XXX maybe an error?

                append(ret, sprintfa(" %s {", suetag(t2)));

                nxt_indent = indent + "  ";

                //an array whose elements are tables
                fldstrs = [];

                for(i=0; i<length(flds); ++i)
                {
                    @local cfld;

                    cfld = [:];
                    append(fldstrs, cfld);

                    cfld.offstr = sprintfa("@0x%x", fieldoff(flds[i]));
                    cfld.typestr = sprintfa("%t", fieldtype(flds[i]));
                    cfld.idstr = sprintfa("%s", fieldid(flds[i]));

                }
                mxoff = apply(max,
                              map(@lambda(c) { length(c.offstr); }, fldstrs));
                mxtype = apply(max,
                               map(@lambda(c) { length(c.typestr); },
                                   fldstrs));
                mxid = apply(max,
                             map(@lambda(c) { length(c.idstr); }, fldstrs));

                for(i=0; i<length(flds); ++i)
                {
                    @local nindent, j, buf, fls;
                    fls = [];

                    append(fls, "\n");
                    nindent = 0;
                    append(fls, nxt_indent);
                    nindent += length(nxt_indent);

                    append(fls, fldstrs[i].offstr);
                    for (j=length(fldstrs[i].offstr); j < mxoff; ++j) {
                        append(fls, " ");
                    }
                    nindent += mxoff;
                    append(fls, "  ");
                    nindent += 2;

                    append(fls, fldstrs[i].typestr);
                    for (j=length(fldstrs[i].typestr); j < mxtype; ++j) {
                        append(fls, " ");
                    }
                    nindent += mxtype;
                    append(fls, " ");
                    nindent += 1;

                    append(fls, fldstrs[i].idstr);
                    for (j=length(fldstrs[i].idstr); j < mxid; ++j) {
                        append(fls, " ");
                    }
                    nindent += mxid;
                    append(fls, " => ");
                    nindent += 4;

                    nindent = mkstr(nindent);
                    memset((char*)nindent,' ',length(nindent));

                    buf = cptrstr_rec(
                            {mkctype_ptr(fieldtype(flds[i]),
                                            nsptr(domof(p).ns))}
                                ((uintptr)p + {domof(p)}fieldoff(flds[i])),
                            newdepth, nindent);

                    if ((strstr(buf, "\n") == nil &&
                         strstr(buf, "...") != nil)
                        || (max_indent != -1 &&
                            length(nindent)+length(split(buf, "\n")[0])
                               > max_indent))
                    {
                        @local bo;
                        nindent = substr(nindent, 0,
                                         length(nindent)-5-mxid-mxtype);
                        bo = buf;
                        buf = cptrstr_rec(
                                {mkctype_ptr(fieldtype(flds[i]),
                                                nsptr(domof(p).ns))}
                                    ((uintptr)p + {domof(p)}fieldoff(flds[i])),
                                newdepth, nindent);
                        if (strstr(buf, "\n") == nil &&
                            strstr(buf, "...") != nil)
                        {
                            buf = bo;
                        } else {
                            append(fls, "\n");
                            append(fls, nindent);
                        }
                    }
                    append(fls, buf);
                    append(fls, ";");
                    ret = concat(ret, fls);
                }
                if (susize(t2) != nil) {
                    append(ret, sprintfa("\n%s@0x%x;", nxt_indent,
                                         susize(t2)));
                    append(ret, sprintfa(" // ends at 0x%x",
                                         (uintptr)p + susize(t2)));
                }
                append(ret, "\n");
                append(ret, indent);
                append(ret, "}");
                return join(ret);
            }
            if (isarray(t2)) {
                @local len, i, nxt_indent, ret, t3, pt3;

                nxt_indent = indent+" ";
                len = arraynelm(t2);
                if (len == nil) return sprintfa("[] // unknown size");

                if (!ismapped(p)) return "UNMAPPED";

                t3 = subtype(t2);
                while(istypedef(t3)) t3 = typedeftype(t3);

                //this is a bit of a hack...
                if (isbase(t3) && baseid(t3) == "char") {
                    return sprintfa("%a", getbytes(p,len));
                }

                pt3 =  mkctype_ptr(subtype(t2), nsptr(domof(p).ns));

                ret = "[";
                for(i=0; i<len; ++i) {
                    if (i != 0) ret += ",\n"+nxt_indent;
                    ret += cptrstr_rec({pt3}&(*p)[i], newdepth, nxt_indent);
                }
                ret += "]";
                return ret;
            }
            if (isptr(t2)) {
                if (!ismapped(p)) return "UNMAPPED";
                return sprintfa("@0x%x -> %s", p,
                                cptrstr_rec(*p, newdepth, indent));
            }
            return sprintfa("@0x%x %t = %s", p, t2, cvalstr(t2,*p));
        }
        return sprintfa("%t %s", t, cvalstr(t,p));
    }

    cptrstr_rec(x, depth, "");

}

