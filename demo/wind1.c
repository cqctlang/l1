static void
f(int x)
{
	if(x == 0)
		return;
	f(x-1);
}

int
main(int argc, char *argv[])
{
	f(0);
	return 0;
}
