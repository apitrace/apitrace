#!/usr/bin/env python3

import os
import sys

os.closerange(3, 10)
os.execv(sys.argv[1], sys.argv[1:])
