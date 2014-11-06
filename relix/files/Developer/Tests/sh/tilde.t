#!/usr/bin/env jtest

$ HOME="/foo/bar" sh -c "echo ~"

1 >= "/foo/bar"

%%

$ HOME="/foo/bar" sh -c "echo ~/"

1 >= "/foo/bar/"

%%

$ HOME="/foo/bar" sh -c "echo ~/baz"

1 >= "/foo/bar/baz"

%%

$ HOME="/foo/bar" sh -c "echo /~"

1 >= "/~"

%%

$ HOME="/foo/bar" sh -c "echo baz~quux"

1 >= "baz~quux"

%%

$ HOME="/foo/bar" sh -c "echo '~/baz'"

1 >= "~/baz"

%%

$ HOME="/foo/bar" sh -c "echo \"~/baz\""

1 >= "~/baz"

%%

$ HOME="/foo/bar" sh -c "echo \'~/baz\'"

1 >= "'~/baz'"

%%

$ HOME="~/foo/bar" sh -c "echo ~"

1 >= "~/foo/bar"

%%

$ HOME="/foo/bar" sh -c "echo \~/baz"

1 >= "~/baz"

%%

$ HOME="*" sh -c "echo ~"

1 >= "*"
