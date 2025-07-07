#!/bin/bash

git submodule foreach --recursive "git reset --hard"
git submodule foreach --recursive "git clean -fd"
git submodule update --init --recursive