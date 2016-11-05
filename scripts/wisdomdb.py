#!/usr/bin/env python3.4

import sys

for f in sys.argv[1:]:
    print("#include \"%s\"" % f)


# EOF
