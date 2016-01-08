vlib Source Code Roadmap
========================

Level 1
-------

`error.*`:  Defines utility macros `SYNTAX_ERROR()` and `INTERNAL_ERROR()`.

`source.*`:  Defines `struct source_spec`, which stores the filename and line number in a source file where a token was seen.

`token_type.*`:  Defines `enum token_type`, with members for each token type, and `next_token_type()`, which scans source text for the next whole token.

`op_type.hh`:  Defines `enum op_type`, with members for each operator.

`proc_info.hh`:  Defines `struct proc_info`, which groups a function pointer with a name.

`type_info.hh`:  Defines `struct type_info`, which provides a type's name.

`vbox.*`:  Defines `class vbox`, a type that generalizes storage of dynamic values, with small-object optimization.

Level 2
-------

`value.*`:  Defines `enum value_type`, `struct Value`, and `struct Expr`.  (Codependent with `symbol.*`.)

`symbol.*`:  Defines `class Symbol`.  (Codependent with `value.*`.)

`token.*`:  Defines `struct token` and `next_token()`, which wrap `token_type` and `next_token_type()` to include the string of text from which the token was scanned.

`ops.*`:  Defines `op_from_token()`, which converts a token and a state flag into an operator.

`named_ops.*`:  Defines `op_from_name()`, which converts a string into a binary infix operator.

`precedence.*`:  Defines `decreasing_op_precedence()`, which implements operator precedence and determines whether a stacked operation can be evaluated immediately, or only after further input.

`quote.*`:  Defines routines for decoding string literals, including escape sequences.

`exceptions.*`:  Defines exception classes.

Level 3
-------

`list-utils.*`:  Defines functions for computations of lists.

`dyad.*`:  Defines `struct dyad`, which holds a left operand and an operator.

`symbol_table.*`:  Defines functions for querying and updating the global symbol table.

`string-utils.*`:  Defines functions for computations of strings.

`iterator.*`:  Defines `class full_iterator` for visiting all known objects.

`stack.*`:  Defines `nth_frame()` for crawling the call stack.

Level 4
-------

`types.*`:  Defines a `type_info` for each V base type.

`symdesc.*`:  Defines a `make_metasymbol()` and `resolve_symbol()`, used with symbol descriptors.

`tracker.*`:  Implements a mark-and-sweep garbage collector.

Level 5
-------

`scope.*`:  Defines `class lexical_scope` (and `class lexical_scope_box`).

`functions.*`:  Defines various built-in V functions.

`library.*`:  Defines various impure library functions.

`calc.*`:  Defines `calc()`, which evaluates a pure binary operation (i.e. without side effects or dependence on run-time state other than its operands).

Level 6
-------

`init.*`:  Defines `define()`, which installs a function as a constant in the symbol table, and `install_basic_symbols()`.

`eval.*`:  Defines `eval()`, which evaluates a binary operation.

Level 7
-------

`parse.*`:  Defines `parse()`, which parses the text of a program into an expression tree.

`execute.*`:  Defines `execute()`, which evaluates an expression tree and returns the result.

Level 8
-------

`interpret.*`:  Defines `interpret()`, which parses and executes a program and reports any errors.
