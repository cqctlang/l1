
@export(f1, f2);

@define f1() { printf("Running f1 from lib3\n"); }
@define f2() {
  f1();
  printf("Running f2 from lib3\n");
}
printf("in lib3\n");
f2();
printf("finished with lib3\n");


