TESTS_ARITH=ad su ml dv
TESTS=${TESTS_ARITH}

RUN_TEST=./run_test.sh

check:
	@for test in $(TESTS); do $(RUN_TEST) test_$$test; done

clean:
	rm -f *.out
