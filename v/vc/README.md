vc
==

`vc` is a simple calculator.

	$ vc '3 + 4'
	7

It supports operator precedence:

	$ vc '3 + 4 * 2'
	11

… as well as parentheses for grouping:

	$ vc '(3 + 4) * 2'
	14

… and arbitrarily large integers:

	$ vc '98765432109876543210 * -98765432109876543210'
	-9754610579850632525677488187778997104100

[Source code roadmap][roadmap]

[roadmap]:  <vc/README.md>

© 2015 Joshua Juran and released under the [GNU AGPL version 3][AGPL] (or later).

[AGPL]:  <../../LICENSE/AGPL-3.0.txt>
