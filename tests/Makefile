TEST_FILES=test_ad
RUN_TEST=./run_test.sh

check:
	@for test_file in $(TEST_FILES); do $(RUN_TEST) $$test_file; done

clean:
	rm -f *.out
