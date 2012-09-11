#!/usr/bin/env python

import sys

if len(sys.argv) < 2:
        print "Usage: %s <iov_list file>" % sys.argv[0]
        print "Check consistency of IOV_start and IOV_stop in IOV lists dumped with"
        print "cmscond_list_iov -c <connection> -t <tag> -o"
        print "and manipulated for usage with cmscond_load_iov"
        sys.exit(1)

cnt = -1
t1, t2 = 0, 0
for l in open(sys.argv[1]):
        cnt += 1
        if cnt < 3:
                continue
        v = l.split()
        ot1 = t1
        ot2 = t2
        t1 = int(v[0])
        t2 = int(v[1])
        if ((t1 >= t2 or ot1 >= ot2 or t1 <= ot2) and cnt > 3):
                print "Problem with line:", l,

