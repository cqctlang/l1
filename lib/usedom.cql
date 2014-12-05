
//XXX needs documentation

@export( usedom );

@define usedom(dom, stx){

	@local cmp, domsym, fn;

	domsym = gensym();

	@defloc walk(e) {
		@local p, l;
		if(e == nil)
			return e;
		switch(stxkind(e)){
		case 'stxunquote:
			error("misplaced unquote");
		case 'stxsplice:
			error("misplaced splice");
		case 'stxquasi:
			error("quasi not allowed");
		case 'stxquote:
			return e;
		case 'val:
			if (iscvalue(stxval(e)))
				return #xcast(domsym,e);
			return e;
		case 'typename:
			if (stxkind(stxref(e,0)) != 'tickt) {
				return #typename(#tickt(domsym, stxref(e,0)),
				                 stxref(e,1));
			}
			return e;
		case 'arrow:
			return #arrow(walk(stxref(e,0)),stxref(e,1));
		case 'dot:
			return #dot(walk(stxref(e,0)),stxref(e,1));
		case 'id:
			if (dom.looksym(stxid(e)) != nil) {
				return #ticke(domsym,e);
			}
			return e;
		case 'elist:
			p = e;
			l = Znull();
			while(stxkind(p) == 'elist){
				l = Zcons(walk(Zcar(p)), l);
				p = Zcdr(p);
			}
			return Zreverse(l);
		default:
			return mkstx(stxkind(e),
				     walk(stxref(e, 0)),
				     walk(stxref(e, 1)),
				     walk(stxref(e, 2)),
				     walk(stxref(e, 3)),
				     stxsrc(e));
		}
	}

	stx = #lambda(#[ domsym ], walk(stx));

	cmp = compile(stx);

	if (cmp == nil)
		error("bad syntax");

	fn = cmp();

	return fn(dom);
}


// vim:ts=8:sw=8:noet
