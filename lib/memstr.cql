@local max_indent, max_string;

//if you're looking here for more information about this library, you may
//also want to look at the test code, which provides example usage.
//see <l1>/test/memstr.l1

@export( cptrstr, setmaxindent, setmaxstring );

@defloc cvalstr(t,v) {
    if (isenum(t) || isenumconst(t)) {
        return sprintfa("%e (%d)",{t}v,v);
    }
    return sprintfa("%a",v);
}

max_indent = 40;
max_string = 50;

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

    @defloc handler_char(p,i,cs) {
        @local str;
        if (!ismapped(p)) {
            str = "UNMAPPED";
        } else {
            str = stringof(p);
        }
        if (max_string != -1 && length(str) > max_string) {
            str = substr(str, 0, max_string) + "...";
        }
        return sprintfa("%t at 0x%x = %a",@typeof(p),p,str);
    }

    //default handlers
    if (handlers[@typeof(dom`char)] == nil) {
        handlers[@typeof(dom`char)] = handler_char;
    }

    @defloc handler_void(p,i,cs) {
        return sprintfa("%t at 0x%x",@typeof(p),p);
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

            if (depth == 0) {
                return "...";
            }
            if (depth == -1) {
                newdepth = -1;
            } else {
                newdepth = depth - 1;
            }

            t2 = subtype(t);

            if (handlers[t2] != nil) {
                return dohandler(t2, p, newdepth, indent);
            }

            while(istypedef(t2)) {
                t2 = typedeftype(t2);
                if (handlers[t2] != nil) {
                    return dohandler(t2, p, newdepth, indent);
                }
            }

            if (isvoid(t2)) {
                return sprintfa("void* at 0x%x",p);
            }

            if (issu(t2)) {
                @local flds, i, nxt_indent, ret, size;

                if (!ismapped(p)) return "UNMAPPED";

                if (max_indent != -1 && strlen(indent)>max_indent) {
                    return sprintfa("...", p);
                }
                flds = fields(t2);
                if (isstruct(t2)) ret = "struct ";
                else if (isunion(t2)) ret = "union ";
                else ret = "aggregate "; //XXX maybe an error?

                if (susize(t2) != nil) {
                    size = sprintfa("(size 0x%x) ", susize(t2));
                } else size = "";

                ret += sprintfa("%s at 0x%x %s{", suetag(t2), p, size);
                for(i=0; i<length(flds); ++i)
                {
                    @local buf;

                    buf = sprintfa("\n%s%s: ",
                            indent + "   ",
                            fieldid(flds[i]));
                    nxt_indent = mkstr(strlen(buf)-1);
                    memset((char*)nxt_indent,' ',length(nxt_indent));
                    ret += buf + cptrstr_rec(
                            {mkctype_ptr(fieldtype(flds[i]),
                                            nsptr(domof(p).ns))}
                                ((uintptr)p + {domof(p)}fieldoff(flds[i])),
                            depth, /* use depth not newdepth here b/c we
                                      are adding an artificial ptr above */
                            nxt_indent);
                }
                ret += sprintfa("\n%s}",indent);
                return ret;
            } else if (isarray(t2)) {
                @local len, i, nxt_indent, ret, t3;

                nxt_indent = indent+" ";
                len = arraynelm(t2);
                if (len == nil) return sprintfa("[] // unknown size");

                if (!ismapped(p)) return "UNMAPPED";

                t3 = subtype(t2);
                while(istypedef(t3)) t3 = typedeftype(t3);

                if (isbase(t3) && baseid(t3) == "char") {
                    return sprintfa("%a",getbytes(p,len));
                }

                ret = "[";
                for(i=0; i<len; ++i) {
                    if (i != 0) ret += ",\n"+nxt_indent;
                    ret += cptrstr_rec(&(*p)[i], newdepth, nxt_indent);
                }
                ret += "]";
                return ret;
            } else if (isptr(t2)) {
                if (!ismapped(p)) return "UNMAPPED";
                return cptrstr_rec(*p, newdepth, indent);
            } else if (isfunc(t2)) {
                return sprintfa("0x%x",p);
            } else if (isbitfield(t2)) {
                @local str;
                if (!ismapped((char*)p, sizeof(bitfieldcontainer(t2)))) {
                    str = "UNMAPPED";
                } else {
                    str = cvalstr(t2, *p);
                }
                return sprintfa("bitfield at 0x%x:[0x%x:0x%x] = %s",
                                p, bitfieldpos(t2), bitfieldwidth(t2),
                                str);
            }
            return sprintfa("%t at 0x%x = %s",t2,p,cvalstr(t2,*p));
        }
        return sprintfa("%t %s",t,cvalstr(t,p));
    }

    cptrstr_rec(x, depth, "");

}

