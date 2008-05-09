#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

void
compile0(Expr *e)
{
	Expr *se, *te;

	if(e == NULL)
		return;

	switch(e->kind){
	case Elambda:
	case Eblock:
		compile0(e->e2);
		break;
	case Etick:
		e->kind = Eblock;

		/* local bindings are list of identifier lists */
		te = nullelist();
		te = newexpr(Eelist, doid("p"), te, 0, 0);
		te = newexpr(Eelist, doid("t"), te, 0, 0);
		te = newexpr(Eelist, doid("b"), te, 0, 0);
		e->e1 = newexpr(Eelist, te, nullelist(), 0, 0);

		te = nullelist();

		// p = dispatch(!looksym, <sym>);
		se = newexpr(Econsts, 0, 0, 0, 0);
		se->lits = e->lits;
		e->lits = 0;
		se = newexpr(Eelist, se, nullelist(), 0, 0);
		se = newexpr(Eelist, doid("!looksym"), se, 0, 0);
		se = newexpr(Ecall, doid("dispatch"), invert(se), 0, 0);
		se = newexpr(Eg, doid("p"), se, 0, 0);
		te = newexpr(Eelist, se, te, 0, 0);

		// t = car(p);
		se = newexpr(E_car, doid("p"), 0, 0, 0);
		se = newexpr(Eg, doid("t"), se, 0, 0);
		te = newexpr(Eelist, se, te, 0, 0);

		// b = dispatch(!get, range(cdr(p), sizeof(t)));
		se = newbinop(E_range,
			      newexpr(E_cdr, doid("p"), 0, 0, 0),
			      newexpr(E_sizeof, doid("t"), 0, 0, 0));
		se = newexpr(Eelist, se, nullelist(), 0, 0);
		se = newexpr(Eelist, doid("!get"), se, 0, 0);
		se = newexpr(Ecall, doid("dispatch"), invert(se), 0, 0);
		se = newexpr(Eg, doid("b"), se, 0, 0);
		te = newexpr(Eelist, se, te, 0, 0);
		
		// cval(b, t);
		se = newbinop(E_cval, doid("b"), doid("t"));
		te = newexpr(Eelist, se, te, 0, 0);

		e->e2 = invert(te);
		break;
	default:
		compile0(e->e1);
		compile0(e->e2);
		compile0(e->e3);
		compile0(e->e4);
		break;
	}
}

