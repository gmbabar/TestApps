#!/usr/bin/python

import sys, getopt
import subprocess
import os

def usage():
    print sys.argv[0], "-n <count>"

def main(argv):
    nc = 0
    try:
        opts, args = getopt.getopt(argv,"hn:",["n=",])
    except getopt.GetoptError:
        usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            usage()
            sys.exit()
        elif opt in ("-n", "--nclient"):
            nc = int(arg)
        else:
            usage()
            sys.exit(2)
    print 'Number of client instances to launch: ', nc
    base_id = 1000
    filename = "/tmp/_test.log"
    for x in range(1,nc+1):
        id_ = base_id + x
        print "Launcing...", id_
        cmd = "./TcpSocket --client >> " + filename + " 2>&1 &"
        print cmd
        os.system( cmd )

if __name__ == "__main__":
    if len(sys.argv) < 2:
        usage()
        sys.exit(2)
    main(sys.argv[1:])

