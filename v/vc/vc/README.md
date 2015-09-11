vc Source Code Roadmap
======================

Level 0
-------

`error.*`:  Defines utility macros `SYNTAX_ERROR()` and `INTERNAL_ERROR()`.

`token_type.*`:  Defines `enum token_type`, with members for each token type, and `next_token_type()`, which scans source text for the next whole token.

`op_type.hh`:  Defines `enum op_type`, with members for each operator.

`function_id.hh`:  Defines `enum function_id`, with members for each function.

`value.*`:  Defines `enum value_type` and `struct Value`.

Level 1
-------

`token.*`:  Defines `struct token` and `next_token()`, which wrap `token_type` and `next_token_type()` to include the string of text from which the token was scanned.

`ops.*`:  Defines `op_from_token()`, which converts a token and a state flag into an operator.

`named_ops.*`:  Defines `op_from_name()`, which converts a string into a binary infix operator.

`functions.*`:  Defines `function_from_name()`, which converts a string into a function ID.

`dyad.*`:  Defines `struct dyad`, which holds a left operand and an operator.

`eval.*`:  Defines `eval()`, which evaluates a binary operation.

`precedence.*`:  Defines `decreasing_op_precedence()`, which implements operator precedence and determines whether a stacked operation can be evaluated immediately, or only after further input.

Level 2
-------

`parse.*`:  Defines `parse_and_eval()`, which parses the text of an arithmetic expression, evaluates it, and returns the result.
