@local max_indent, max_string;

@export( cptrstr, setmaxindent, setmaxstring );

@defloc cvalstr(t,v) {
    if (isenum(t) || isenumconst(t)) {
        return sprintfa("%e (%d)",{t}v,v);
    }
    return sprintfa("%a",v);
}

max_indent = 40;
max_string = 50;

@define setmaxindent(x) {
    @local t;

    if (!isctype(x)) error("must provide an integer");
    t = @typeof(x);
    if (!isbase(t) || basebase(t) != cqct`Vint)
        error("must provide an integer");
    max_indent = x;
}

@define setmaxstring(x) {
    @local t;

    if (!isctype(x)) error("must provide an integer");
    t = @typeof(x);
    if (!isbase(t) || basebase(t) != cqct`Vint)
        error("must provide an integer");
    max_string = x;
}

/**
* returns a string representation of the object pointed to by x (including
* its values in memory).
* params: x is a pointer to an object
* handlers: options table
* returns: a string representation of x and its content
*/
@define cptrstr(x,handlers...)
{
    @local dom;

    if (length(handlers)>0) {
        handlers = head(handlers);
        if (!istable(handlers)) {
            error("Second parameter to cptrstr must be a table");
        }
    } else handlers = [:];

    dom = domof(x);

    @defloc handler_char(p,i,cs) {
        @local str;
        str = stringof(p);
        if (length(str) > max_string) {
            str = substr(str, 0, max_string) + "...";
        }
        //XXX what if p is unmapped?
        return sprintfa("%t at 0x%x = %a",@typeof(p),p,str);
    }
    //default handlers (currently only char*)
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
    @defloc dohandler(t,p,indent) {
        @local fn;
        fn = handlers[t];
        return dynamicwind(
                @lambda() { delete(handlers,t); },
                @lambda() { fn(p, indent,
                               @lambda(p) { cptrstr_rec(p, indent); }); },
                @lambda() { handlers[t] = fn; }
              );
    }

    @defloc cptrstr_rec(p,indent) {
        @local t;

        //indent is the amount to indent all except the first line
        //(the first line will be appropriately indented by the caller)

        t = @typeof(p);

        if (handlers[t] != nil) {
            return dohandler(t,p,indent);
        }

        while(istypedef(t)) {
            t = typedeftype(t);
            if (handlers[t] != nil) {
                return dohandler(t,p,indent);
            }
        }

        if (isptr(t)) {
            @local t2;

            t2 = subtype(t);

            if (handlers[t2] != nil) {
                return dohandler(t2,p,indent);
            }

            while(istypedef(t2)) {
                t2 = typedeftype(t2);
                if (handlers[t2] != nil) {
                    return dohandler(t2,p,indent);
                }
            }

            if (isvoid(t2)) {
                return sprintfa("void* at 0x%x",p);
            }

            if (issu(t2)) {
                @local flds, i, nxt_indent, ret, size;

                if (!ismapped(p)) return "UNMAPPED";

                if (strlen(indent)>max_indent ) return sprintfa("...",p);
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
                    ret += cptrstr_rec(&(*p)[i],nxt_indent);
                }
                ret += "]";
                return ret;
            } else if (isptr(t2)) {
                if (!ismapped(p)) return "UNMAPPED";
                return cptrstr_rec(*p,indent);
            } else if (isfunc(t2)) {
                return sprintfa("0x%x",p);
            } else if (isbitfield(t2)) {
                return sprintfa("bitfield at 0x%x:[0x%x:0x%x] = %s",
                                p, bitfieldpos(t2), bitfieldwidth(t2),
                                cvalstr(t2,*p));
            }
            return sprintfa("%t at 0x%x = %s",t2,p,cvalstr(t2,*p));
        }
        return sprintfa("%t %s",t,cvalstr(t,p));
    }

    cptrstr_rec(x,"");

}

