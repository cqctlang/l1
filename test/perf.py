#!/usr/bin/env python

import glob
import os
import popen2
import sys
import platform
import time

os.putenv('GIT_PAGER', 'cat');
p = popen2.Popen4('git show --pretty=oneline');
rv = p.wait()
if rv == 0:
    commit = p.fromchild.readlines()[0].rstrip()
else:
    commit = 'commit unknown'
now = time.strftime("%Y.%2m.%2d.%2H.%2M.%2S", time.localtime())
uname = os.uname()
fn = 'perfdat/'+now

f = open(fn, 'w')

print >>f, commit
print >>f, now
print >>f, uname[0]
print >>f, uname[1]
print >>f, uname[2]
print >>f, uname[3]
print >>f, uname[4]


fault = []

print >>f, '---'
for m in glob.glob('[A-Za-z0-9]*.l1'):
    sys.stdout.flush()
    cmd = '../l1 -t -z '+m
    p = popen2.Popen4(cmd);
    rv = p.wait()
    if rv != 0:
        print 'fault'
        fault.append(m);
        continue
    out = p.fromchild.readlines()
    del p
    print >>f, out[-1].rstrip(), '\t', m

f.close()
print fn

if len(fault) > 0:
    print 'fault:'
    for m in fault:
        print '\t', m

