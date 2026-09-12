DES test scripts
================

Synopsis:

	./build.pl -i des-test-helper des-cbc-test-helper minivx
	cd crypto/des/
	PATH=../../var/bin:$PATH bin/des-text.vx data/des_test_vectors.txt
	PATH=../../var/bin:$PATH bin/des-cbc-text.vx data/des-cbc
