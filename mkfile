< /$objtype/mkfile

TARG=l1
OFILES=\
	hash.$O\
	util.$O\
	printexpr.$O\
	bitfield.$O\
	parse.$O\
	xenv.$O\
	cutil.$O\
	compilens.$O\
	compilea.$O\
	compile0.$O\
	compile1.$O\
	compile2.$O\
	compilev.$O\
	spec.$O\
	type.$O\
	cg.$O\
	vm.$O\
	cqct.$O\
	lex.yy.$O\
	c.tab.$O\

</sys/src/cmd/mkone
CFLAGS=-FTVwp

clean:V:
	rm -f *.[$OS] [$OS].out $TARG
