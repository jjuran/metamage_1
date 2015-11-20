vlib Source Code Roadmap
========================

Level 1
-------

`error.*`:  Defines utility macros `SYNTAX_ERROR()` and `INTERNAL_ERROR()`.

`token_type.*`:  Defines `enum token_type`, with members for each token type, and `next_token_type()`, which scans source text for the next whole token.

`op_type.hh`:  Defines `enum op_type`, with members for each operator.

`symbol_id.hh`:  Defines `symbol_id` as an alias of `Symbol*`.

`proc_info.hh`:  Defines `struct proc_info`, which groups a function pointer with a name.

`type_info.hh`:  Defines `struct type_info`, which provides a type's name.

`vbox.*`:  Defines `class vbox`, a type that generalizes storage of dynamic values, with small-object optimization.

Level 2
-------

`value.*`:  Defines `enum value_type`, `struct Value`, and `struct Expr`.

`token.*`:  Defines `struct token` and `next_token()`, which wrap `token_type` and `next_token_type()` to include the string of text from which the token was scanned.

`ops.*`:  Defines `op_from_token()`, which converts a token and a state flag into an operator.

`named_ops.*`:  Defines `op_from_name()`, which converts a string into a binary infix operator.

`precedence.*`:  Defines `decreasing_op_precedence()`, which implements operator precedence and determines whether a stacked operation can be evaluated immediately, or only after further input.

`quote.*`:  Defines routines for decoding string literals, including escape sequences.

Level 3
-------

`symbol_table.*`:  Defines functions for querying and updating the global symbol table.

`string-utils.*`:  Defines functions for computations of strings.

`list-utils.*`:  Defines functions for computations of lists.

`dyad.*`:  Defines `struct dyad`, which holds a left operand and an operator.

Level 4
-------

`types.*`:  Defines a `type_info` for each V base type.

`functions.*`:  Defines various built-in V functions.

`library.*`:  Defines various library functions, and `define()` for installing them.

Level 5
-------

`calc.*`:  Defines `calc()`, which evaluates a pure binary operation (i.e. without side effects or dependence on run-time state other than its operands).

Level 6
-------

`init.*`:  Defines `define()`, which installs a function as a constant in the symbol table, and `install_basic_functions()`.

`eval.*`:  Defines `eval()`, which evaluates a binary operation.

Level 7
-------

`parse.*`:  Defines `parse_and_eval()`, which parses the text of an arithmetic expression, evaluates it, and returns the result.
