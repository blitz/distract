# -*- Mode: Python -*-

import os

env = Environment(CXX      = 'clang++',
                  CXXFLAGS = [ '-O2', '-std=c++11' ],
                  LIBS     = [ 'boost_system' ])

# This override is for travis
if "COMPILER" in os.environ:
    env['CXX'] = os.environ["COMPILER"]

env.ParseConfig('pkg-config --cflags --libs gloox')

env.Command('wisdomdb.inc', Glob('wisdom/*.inc'), "./scripts/wisdomdb.py $SOURCES > $TARGET")

env.Program('distract', Glob('*.cpp'))

# EOF
