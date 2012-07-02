@dynlib(lib1,
  [ (f3, lib2.f1), (f4, lib2.f4) ],
  [f1, f2]) 
{ 
  @define f1() { printf("Running f1 from lib1\n"); }
  @define f2() { 
    f1();
    f3();
    f4();
    printf("Running f2 from lib1\n"); 
  }
  printf("in lib1\n");
  f2();
  printf("finished with lib1\n");
}

