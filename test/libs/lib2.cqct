@dynlib(lib2,
  [ (f5, lib1.f1) ],
  [f1, f4])
{
  @define f1() { printf("Running f1 from lib2\n"); }
  @define f4() { printf("Running f4 from lib2\n"); }
  printf("In lib2: "); f5();
  printf("Finished lib2\n");
}
