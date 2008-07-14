#!/usr/bin/env /usr/bin/python

import glob
import os
import popen2
import sys

nok = 0
nnew = 0
nfail = 0
nfault = 0

for m in glob.glob('*.l1'):
    print m,
    sys.stdout.flush()
#    f = open(m);
#    s = f.read();
#    f.close()
#    p = popen2.Popen4('../l1')
#    p.tochild.write(s)
#    p.tochild.close()
    cmd = '../l1 -b -e '+m
    p = popen2.Popen4(cmd);
    rv = p.wait()
    if rv != 0:
        print 'fault'
        nfault = nfault+1
        continue
    out = p.fromchild.read()
    del p
    fn = m+'.out'
    if os.path.exists(fn):
        f = open(fn)
        s = f.read()
        f.close()
        if s == out:
            print 'ok'
            nok = nok+1
        else:
            f = open(m+'.failed','w')
            f.write(out)
            f.close
            print 'failed'
            nfail = nfail+1
    else:
        f = open(fn,'w')
        f.write(out)
        f.close()
        print 'new'
        nnew = nnew+1

print nok, 'ok', nnew, 'new', nfault, 'fault', nfail, 'fail'
