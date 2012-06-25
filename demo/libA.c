static int
a1(int y)
{
	return y+2;
}

int
a0(int x)
{
	int y, z;
	void *p;  /* use a pointer type so sctl knows ptr size */
	p = 0;
	y = x+1;
	z = a1(y);
	return z+1;
}
