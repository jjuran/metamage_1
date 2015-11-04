vlib Source Code Roadmap
========================

Level 0
-------

`error.*`:  Defines utility macros `SYNTAX_ERROR()` and `INTERNAL_ERROR()`.

`token_type.*`:  Defines `enum token_type`, with members for each token type, and `next_token_type()`, which scans source text for the next whole token.

`op_type.hh`:  Defines `enum op_type`, with members for each operator.

`symbol_id.hh`:  Defines `enum symbol_id`, the type of an index into the symbol table.

`proc_info.hh`:  Defines `struct proc_info`, which groups a function pointer with a name.

Level 1
-------

`value.*`:  Defines `enum value_type`, `struct Value`, and `struct Expr`.

`expr_box.*`:  Defines `class expr_box`, an intrusive smart pointer to an `Expr`.

`token.*`:  Defines `struct token` and `next_token()`, which wrap `token_type` and `next_token_type()` to include the string of text from which the token was scanned.

`ops.*`:  Defines `op_from_token()`, which converts a token and a state flag into an operator.

`named_ops.*`:  Defines `op_from_name()`, which converts a string into a binary infix operator.

`precedence.*`:  Defines `decreasing_op_precedence()`, which implements operator precedence and determines whether a stacked operation can be evaluated immediately, or only after further input.

`quote.*`:  Defines routines for decoding string literals, including escape sequences.

Level 2
-------

`string-utils.*`:  Defines functions for computations of strings.

`list-utils.*`:  Defines functions for computations of lists.

`symbol_table.*`:  Defines functions for querying and updating the global symbol table.

`dyad.*`:  Defines `struct dyad`, which holds a left operand and an operator.

Level 3
-------

`functions.*`:  Defines `function_from_name()`, which converts a string into a function pointer.

`calc.*`:  Defines `calc()`, which evaluates a pure binary operation (i.e. without side effects or dependence on run-time state other than its operands).

`library.*`:  Defines various library functions, and `define()` for installing them.

Level 4
-------

`eval.*`:  Defines `eval()`, which evaluates a binary operation.

Level 5
-------

`parse.*`:  Defines `parse_and_eval()`, which parses the text of an arithmetic expression, evaluates it, and returns the result.
