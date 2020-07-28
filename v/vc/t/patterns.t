#!/usr/bin/env jtest

$ vc 'var p = begin "112358"; var q = p += "0"?; p.rest, q.rest'
1 >= '("112358", "112358")'

%

$ vc 'var p = begin "112358"; var q = p += "0"*; p.rest, q.rest'
1 >= '("112358", "112358")'

%

$ vc 'var p = begin "112358"; var q = p += "0"+; p.rest, q'
1 >= '"112358"'

%

$ vc 'var p = begin "112358"; var q = p += "1"?; p.rest, q.rest'
1 >= '("12358", "12358")'

%

$ vc 'var p = begin "112358"; var q = p += "1"*; p.rest, q.rest'
1 >= '("2358", "2358")'

%

$ vc 'var p = begin "112358"; var q = p += "1"+; p.rest, q.rest'
1 >= '("2358", "2358")'

%

$ vc 'var p = begin "112358"; var q = p += (*"0" .. *"2")?; p.rest, q.rest'
1 >= '("12358", "12358")'

%

$ vc 'var p = begin "112358"; var q = p += (*"0" .. *"2")*; p.rest, q.rest'
1 >= '("358", "358")'

%

$ vc 'var p = begin "112358"; var q = p += (*"0" .. *"2")+; p.rest, q.rest'
1 >= '("358", "358")'

%

$ vc 'var p = begin "112358"; var q = p += (*"0" .. *"9")?; p.rest, q.rest'
1 >= '("12358", "12358")'

%

$ vc 'var p = begin "112358"; var q = p += (*"0" .. *"9")*; p.rest, q.rest'
1 >= '("", "")'

%

$ vc 'var p = begin "112358"; var q = p += (*"0" .. *"9")+; p.rest, q.rest'
1 >= '("", "")'

%

$ vc 'var p = begin "112358"; var q = p += (*"0" .. *"3" | *"5")?; p.rest, q.rest'
1 >= '("12358", "12358")'

%

$ vc 'var p = begin "112358"; var q = p += (*"0" .. *"3" | *"5")*; p.rest, q.rest'
1 >= '("8", "8")'

%

$ vc 'var p = begin "112358"; var q = p += (*"0" .. *"3" | *"5")+; p.rest, q.rest'
1 >= '("8", "8")'

%

$ vc 'var p = begin "aaaaab"; var q = p += "aa"?; p.rest, q.rest'
1 >= '("aaab", "aaab")'

%

$ vc 'var p = begin "aaaaab"; var q = p += "aa"*; p.rest, q.rest'
1 >= '("ab", "ab")'

%

$ vc 'var p = begin "aaaaab"; var q = p += "aa"+; p.rest, q.rest'
1 >= '("ab", "ab")'

%

$ vc 'var p = begin "abaaaab"; var q = p += "aa"?; p.rest, q.rest'
1 >= '("abaaaab", "abaaaab")'

%

$ vc 'var p = begin "abaaaab"; var q = p += "aa"*; p.rest, q.rest'
1 >= '("abaaaab", "abaaaab")'

%

$ vc 'var p = begin "abaaaab"; var q = p += "aa"+; p.rest, q'
1 >= '"abaaaab"'

%

$ vc 'var p = begin "foobar"; var q = p += "foo" | "fooba"; p.rest, q.rest'
1 >= '("bar", "bar")'

%

$ vc 'var p = begin "foobar"; var q = p += "bar" | "foo"; p.rest, q.rest'
1 >= '("bar", "bar")'

%

$ vc 'var p = begin "foobar"; var q = p += "bar" | "baz"; p.rest, q'
1 >= '"foobar"'

%

$ vc 'var p = begin "foobarbaz"; var q = p += ("foo" | "bar")*; p.rest, q.rest'
1 >= '("baz", "baz")'

%

$ vc 'var p = begin "321--"; var q = p += (*"0" .. *"9")+ | "-"; p.rest, q.rest'
1 >= '("--", "--")'

%

$ vc 'var p = begin "--321"; var q = p += (*"0" .. *"9")+ | "-"; p.rest, q.rest'
1 >= '("-321", "-321")'

%

$ vc 'var p = begin "24601?"; var q = p += (*"0" .. *"9")+ => int; p.rest, q'
1 >= '("?", 24601)'

%

$ vc 'var p = begin "foo bar."; var q = p += (((*"a" .. *"z")+ => str) | " ")+; p.rest, q'
1 >= '(".", "foo", "bar")'

%

$ vc 'var p = begin "foo bar."; var q = p += (((*"a" .. *"z")+ => str) | " ")+ => str; p.rest, q'
1 >= '(".", "foobar")'

%

$ vc 'var p = begin "aaaaacccd"; var q = p += ["a"*, "b"*, "c"*]; p.rest, q.rest'
1 >= '("d", "d")'

%

$ vc 'var p = begin "aaaaacccd"; var q = p += ["a"+, "b"+, "c"+] | ["a"+, "c"]; p.rest, q.rest'
1 >= '("ccd", "ccd")'

%

$ vc 'var p = begin "&#38;"; var q = p += ["&#", (*"0" .. *"9")+ => str * byte * int, ";"]; p.rest, q'
1 >= '("", "&")'

%

$ vc 'var p = begin "foo & bar&#33;"; var q = p += (["&#", (*"0" .. *"9")+ => str * byte * int, ";"] | (*" " .. *"~" => str))+ => str; p.rest, q'
1 >= '("", "foo & bar!")'
