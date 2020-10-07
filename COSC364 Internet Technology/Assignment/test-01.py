#!/usr/bin/env python

import sys
from tester import Tester

test = Tester(sys.argv[1], timer=5)

test.add_router(1)
test.add_router(2)
test.add_router(3)

test.add_link(1, 2, 5)
test.add_link(1, 3, 2)
test.add_link(2, 3, 8)

test.generate_configs()
test.generate_iptables_scripts()
test.start()
