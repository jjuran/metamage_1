#!/usr/bin/env jtest

$ chess-test Nxf3
1 >= "false claim of capture"

$ chess-test e4+
1 >= "false claim of check"

$ chess-test Nf3+
1 >= "false claim of check"

$ chess-test e4#
1 >= "false claim of checkmate"

$ chess-test Nf3#
1 >= "false claim of checkmate"

$ chess-test e4 e5 Qh5 Nf6 Qf7#
1 >= "false claim of checkmate"

$ chess-test e4 d5 exd5 e.p.
1 >= "false claim of en passant"

$ chess-test e4 d5 exd5 a6 d4 e5 dxe5 e.p.
1 >= "false claim of en passant"
