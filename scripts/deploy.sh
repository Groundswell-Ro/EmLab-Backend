#!/bin/bash

echo '------ ssh starts here -----'
ssh evntmgr@tstback '
ls && \
rm -fr ./*
echo "all files removed from tstback" && \
ls
'

echo '------ DEPLOY -----'
ls
scp backend evntmgr@tstback:./
scp comunication.txt evntmgr@tstback:./
scp makefile evntmgr@tstback:./
