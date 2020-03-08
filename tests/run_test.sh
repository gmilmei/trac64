#!/bin/sh
TRAC="../src/trac64 -q"
TEST=$1
TEST_OK=${TEST}.ok
TEST_OUT=${TEST}.out
$TRAC $TEST > ${TEST_OUT}
if cmp ${TEST_OK} ${TEST_OUT}
then
    echo ${TEST}: [32mPASSED[39m
else
    echo ${TEST}: [31mFAIL[39m
fi
