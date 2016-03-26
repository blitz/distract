# -*- Mode: Python -*-

env = Environment(CXX      = 'clang++',
                  CXXFLAGS = [ '-O2', '-std=c++11' ],
                  LIBS     = [ 'boost_system' ])

env.ParseConfig('pkg-config --cflags --libs gloox')

env.Command('wisdomdb.inc', Glob('wisdom/*.inc'), "./scripts/wisdomdb.py $SOURCES > $TARGET")

env.Program('distract', Glob('*.cpp'))

# EOF
