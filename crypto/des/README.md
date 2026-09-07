A vulgar DES implementation
===========================

It's **vulgar** in the sense that it's focused entirely on producing
the correct mathematical result, without regard for informational security.

For example, keys are not cleared from memory after use.
Neither is decrypted plaintext.

The rationale for this nonchalance is that DES is woefully insecure on its own,
and any secrets it guards should be considered already compromised.
