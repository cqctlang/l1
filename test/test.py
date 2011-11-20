#!/usr/bin/env python

import glob
import os
import popen2
import sys

os.chdir(os.path.abspath(os.path.dirname(sys.argv[0])))

ok = []
new = []
fail = []
fault = []

if len(sys.argv) > 1:
    sys.argv.pop(0)
    run = sys.argv
else:
    run = glob.glob('[A-Za-z0-9]*.l1')

for m in run:
    print m,
    sys.stdout.flush()
    cmd = '../l1 -b -w -K '+m
    p = popen2.Popen4(cmd);
    rv = p.wait()
    if os.WIFSIGNALED(rv):
        print 'fault'
        fault.append(m);
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
            ok.append(m);
        else:
            f = open(m+'.failed','w')
            f.write(out)
            f.close
            print 'failed'
            fail.append(m);
    else:
        f = open(fn,'w')
        f.write(out)
        f.close()
        print 'new'
        new.append(m);

print len(ok), 'ok', len(new), 'new', len(fault), 'fault', len(fail), 'fail'

if len(new) > 0:
    print 'new:'
    for m in new:
        print '\t', m

if len(fault) > 0:
    print 'fault:'
    for m in fault:
        print '\t', m

if len(fail) > 0:
    print 'fail:'
    for m in fail:
        print '\t', m

if len(fault) > 0 or len(fail) > 0:
    exit(-1)
