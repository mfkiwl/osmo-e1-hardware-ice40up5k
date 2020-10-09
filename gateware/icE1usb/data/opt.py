#!/usr/bin/env python3

import os
import sys

# Add import path to script provided by no2build
sys.path.insert(0, os.path.join(os.getenv('NO2BUILD_DIR', '../build'), 'sw'))

import ice40_opt_cset
import ice40_opt_lutdup

ice40_opt_cset.run_opt(ctx, threshold=8)
ice40_opt_lutdup.run_opt(ctx)
