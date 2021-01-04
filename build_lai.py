#!/usr/bin/env python3

from buildsystem import builder_json, parse_args

parse_args()

builder_json("build_lai.json").run()

