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

valgrind = os.getenv('HOME')+'/bin/valgrind'
if not os.path.exists(valgrind):
    valgrind = '/usr/bin/valgrind'
if not os.path.exists(valgrind):
    valgrind = '/usr/local/bin/valgrind'
if not os.path.exists(valgrind):
    print 'no valgrind'
    sys.exit();

suppress = ' --suppressions=./pthread-suppression'
vgopt = ' -q --show-reachable=yes --leak-check=full'+suppress
#vgopt = ' -q --tool=exp-ptrcheck'
l1 = ' ../l1 -w '
cmd = valgrind+vgopt+l1

for m in glob.glob('[A-Za-z0-9]*.l1'):
    print m,
    sys.stdout.flush()
    p = popen2.Popen4(cmd+m)
    rv = p.wait()
    if os.WIFSIGNALED(rv):
        print 'fault'
        fault.append(m);
        continue
    out = p.fromchild.read()
    del p
    fn = m+'.vgout'
    if os.path.exists(fn):
        f = open(fn)
        s = f.read()
        f.close()
        if s == out:
            print 'ok'
            ok.append(m);
        else:
            f = open(m+'.vgfailed','w')
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
