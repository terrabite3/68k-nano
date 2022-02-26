#!/bin/bash

cd `dirname $0`

cd ../docker
docker build -t 68k-tools:0.0.4 .