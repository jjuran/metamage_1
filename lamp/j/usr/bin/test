#!/usr/bin/perl
###########################################################################
# test -- Perl script to emulate BSD Unix test(1) functionality
#         (with slightly more informative error messages).
#
# Copyright (c) 1999 Brad Appleton. All rights reserved.
#
# This file is part of "Perl Power Tools: the Unix Reconstruction
# Project" (PPT) which is free software. You can redistribute it
# and/or modify it under the same terms as PPT itself.
#
###########################################################################

package PPT::test;

use strict;
#use diagnostics;
use vars qw($VERSION);

$VERSION = 1.01;

## Set to 0 to disable debug output; non-zero to enable it
my $DEBUG = $ENV{DEBUG_PPT_TEST} || 0;
sub dbval($) {
    local $_ = shift;
    return '<undef>' unless defined;
    return ($_ ? "true" : "false")  if /^\d+$/;
    return (/^-\w/ ? $_ : "'$_'");
}

## Keep track of errors/warnings
my $ERRORS = 0;
$SIG{__WARN__} = sub { print STDERR "test: $_[0]"; ++$ERRORS; };
$SIG{__DIE__}  = sub { print STDERR "test: $_[0]"; exit 2; };

## Complain about an invalid argument
sub bad_arg(@) { warn "invalid argument @_\n"; }

## Make sure we have a numeric value
sub number($) { 
    local $_ = shift;
    return $_ if (defined && /^[-+]?\d+$/);
    bad_arg "'$_[0]' - expecting a number"; 
    return undef;
}

## %TEST_OPS maps test(1) operators or operations to Perl operations
## If a value is a string, the string is returned. If it is a reference,
## then its assumed to be a reference to a subroutine, and the value
## is whatever the subroutine returns.
##
## The code doing the lookup is responsible for passing the correct
## number of parameters and interpreting the result.
##
my %TEST_OPS = (
    ## Logical/grouping operators ('-a' has higher precedence than '-o')
    '('    =>    '(',
    ')'    =>    ')',
    '!'    =>    '!',
    '-a'   =>    '&&',
    '-o'   =>    '||',
       ## NOTE: are && and || 'compatibility' or feature creep?
    '&&'   =>    '&&',
    '||'   =>    '||',

    ## File test operators
    '-b'   =>    sub { -b $_[0] },
    '-c'   =>    sub { -c $_[0] },
    '-d'   =>    sub { -d $_[0] },
    '-e'   =>    sub { -e $_[0] },
    '-f'   =>    sub { -f $_[0] },
    '-g'   =>    sub { -g $_[0] },
    '-h'   =>    sub { -l $_[0] },
    '-k'   =>    sub { -k $_[0] },
    '-l'   =>    sub { -l $_[0] },
    '-p'   =>    sub { -p $_[0] },
    '-r'   =>    sub { -r $_[0] },
    '-s'   =>    sub { -s $_[0] },
    '-t'   =>    sub { -t (number($_[0])||0) },
    '-u'   =>    sub { -u $_[0] },
    '-w'   =>    sub { -w $_[0] },
    '-x'   =>    sub { -x $_[0] },
    '-B'   =>    sub { -B $_[0] },
    '-L'   =>    sub { -l $_[0] },
    '-O'   =>    sub { -O $_[0] },
    '-G'   =>    sub { bad_arg "'-G' - operator not supported", 0 },
    '-R'   =>    sub { -R $_[0] },
    '-S'   =>    sub { -S $_[0] },
    '-T'   =>    sub { -T $_[0] },
    '-W'   =>    sub { -W $_[0] },
    '-X'   =>    sub { -X $_[0] },

    ## String comparisons
    '-n'   =>    sub { length $_[0] },
    '-z'   =>    sub { ! length $_[0] },
    '='    =>    sub { $_[0] eq $_[1] },
    '!='   =>    sub { $_[0] ne $_[1] },
    '<'    =>    sub { $_[0] lt $_[1] },
    '>'    =>    sub { $_[0] gt $_[1] },
       ## NOTE: are ==, <=, and >= 'compatibility' or feature creep?
    '=='   =>    sub { $_[0] eq $_[1] },
    '<='   =>    sub { $_[0] le $_[1] },
    '>='   =>    sub { $_[0] ge $_[1] },

    ## Numeric comparisons
    '-eq'  =>    sub { (number($_[0])||0) == (number($_[1])||0) },
    '-ne'  =>    sub { (number($_[0])||0) != (number($_[1])||0) },
    '-lt'  =>    sub { (number($_[0])||0) <  (number($_[1])||0) },
    '-le'  =>    sub { (number($_[0])||0) <= (number($_[1])||0) },
    '-gt'  =>    sub { (number($_[0])||0) >  (number($_[1])||0) },
    '-ge'  =>    sub { (number($_[0])||0) >= (number($_[1])||0) },

    ## File comparisons
    '-nt'  =>    sub { -M $_[0] < -M $_[1] },
    '-ot'  =>    sub { -M $_[0] > -M $_[1] },
    '-ef'  =>    sub { bad_arg "'-ef' - operator not supported", 0 },
);

## Apply a test operator to the given arguments
sub apply_op ($;@) {
    my $test_op = shift;
    my $perl_op = $TEST_OPS{$test_op};
    my $result  = undef;
    if (defined $perl_op) {
        $result = (ref $perl_op) ? 0+&{$perl_op}(@_) : $perl_op;
    }
    else {
        ## NOTE: should this be an error, or should it be the
        ##       same as saying "-n $test_op"?
        warn "invalid operator '$test_op'\n";
    }
    return $result;
}

## Evaluate test(1) operations and their operands.
## Returns 'undef' upon error, 0 false, !0 for true
sub test (@) {
    my @terms = ();
    local  $_ = "";
    my $grouping  = 0;
    my $need_expr = 1;
    while (@_ > 0) {
        $_ = shift;
        next if m/[\[;\]]/;  ## ignore '[', ']', and ';'

        if (/^[\(\)!]$/) {
            ## grouping and negation
            if ($_ eq ')') {
                --$grouping;
                bad_arg "'$_' - unbalanced parentheses" if ($grouping < 0);
                push @terms, 0  if ($need_expr);
                $need_expr = 0;
            }
            else {
                ++$grouping if ($_ eq '(');
                bad_arg "'$_' - expecting '-a' or '-o'" unless ($need_expr);
            }
            $DEBUG and printf ">>> %s", $_;
            push @terms, apply_op($_);
        }
        elsif (/^(?: -[ao] | \|\| | \&\& )$/x) {
            ## and/or operators
            bad_arg "'$_' - expression expected" if ($need_expr);
            $DEBUG and printf ">>> %s", $_;
            push @terms, apply_op($_);
            $need_expr = 1;
        }
        elsif ($_ eq '-t' and  (@_ == 0 or !defined(number $_[0]))) {
            ## '-t' with no argument
            bad_arg "'$_' - expecting '-a' or '-o'" unless ($need_expr);
            $DEBUG and printf ">>> %s", $_;
            push @terms, 0+(-t );
            $need_expr = 0;
        }
        elsif (/^-\w$/) {
            ## file tests and string length/existence operators
            bad_arg "'$_' - expecting '-a' or '-o'" unless ($need_expr);
            warn "argument expected after '$_'\n" if (@_ == 0);
            if (@_ > 0) {
                $DEBUG and printf ">>> %s %s", $_, $_[0];
                push @terms, apply_op($_, shift);
                $need_expr = 0;
            }
        }
        elsif (@_  and  $_[0] =~ /^(?: -[a-z]{2} | [=!]?= | [<>]=? )$/x) {
            ## We have a string or a filename that participates
            ## in a binary infix operation.
            bad_arg "'$_' - expecting '-a' or '-o'" unless ($need_expr);
            warn "argument expected after '${\shift()}'\n"  if (@_ < 2);
            if (@_ >= 2) {
                $DEBUG and printf ">>> %s %s %s", $_, @_[0..1];
                push @terms, apply_op(shift, $_, shift);
                $need_expr = 0;
            }
        }
        else {
            ## We have a lone string, so "-n string" is implied.
            bad_arg "'$_' - expecting '-a' or '-o'" unless ($need_expr);
            $DEBUG and printf ">>> %s", dbval $_;
            push @terms, length;
            $need_expr = 0;
        }
        $DEBUG and @terms and printf " ==> %s\n", dbval($terms[-1]);
    }

    ## Check for errors so far
    warn "too many open '(' parentheses\n" if ($grouping > 0);
    return undef if $ERRORS;

    ## Now we have all our primitive terms evaluated, compute the result.
    $DEBUG and print ">>>expression is: @terms\n";
    return $terms[0]  if (@terms == 1  and  $terms[0] =~ /^\d*$/);
    my $result = eval "@terms";
    warn "$@" if ($@ and !$ERRORS);
    return ($@) ? undef : 0+$result;
}

exit(2) if (@ARGV == 0);
my $rc = test @ARGV;

## For command-lines, zero is success and non-zero is false, so
## we need to interpret the return code into an exit status.
exit(defined($rc) ? !$rc : 2);

__END__

=head1 NAME

test - condition evaluation utility

=head1 SYNOPSIS

  test expression
  [ expression ]

=head1 DESCRIPTION

The B<test> utility evaluates the I<expression> and, if it evaluates
to true, returns a zero (true) exit status; otherwise it returns 1
(false). If no expression is given, B<test> also returns 1 (false).

All operators and flags are separate arguments to the test utility.

The following primaries are used to construct expression:

=over 14

=item B<-b> I<file>

True if file exists and is a block special file.

=item B<-c> I<file>

True if file exists and is a character special file.

=item B<-d> I<file>

True if file exists and is a directory.

=item B<-e> I<file>

True if file exists (regardless of type).

=item B<-f> I<file>

True if file exists and is a regular file.

=item B<-g> I<file>

True if file exists and its set-group-ID flag is set.

=item B<-h> I<file>

True if file exists and is a symbolic link.  This operator is for
COMPATABILITY purposes, do not rely on its existence.  Use B<-L> instead.

=item B<-k> I<file>

True if file exists and its sticky bit is set.

=item B<-n> I<string>

True if the length of string is nonzero.

=item B<-p> I<file>

True if file is a named pipe (FIFO).

=item B<-r> I<file>

True if file exists and is readable by the effective user/group.

=item B<-s> I<file>

True if file exists and has a size greater than zero.

=item B<-t> I<file_descriptor>

True if the file whose file descriptor number is I<file_descriptor> is
open and is associated with a terminal.

=item B<-t>

Same as above with an implicit file descriptor number of "1"
(e.g.: B<-t> 1).

=item B<-u> I<file>

True if file exists and its set-user-ID flag is set.

=item B<-w> I<file>

True if file exists and is writable by the effective user/group.  True
indicates only that the write flag is on.  The file is not writable on
a read-only file system even if this test indicates true.

=item B<-x> I<file>

True if file exists and is executable by the effective user/group.
True indicates only that the execute flag is on.  If file is a
directory, true indicates that file can be searched.

=item B<-z> I<string>

True if the length of string is zero.

=item B<-B> I<file>

True if file exists and is a binary file.

=item B<-L> I<file>

True if file exists and is a symbolic link.

=item B<-O> I<file>

True if file exists and its owner matches the effective user ID of this
process.

=begin _NOT_IMPLEMENTED_

=item B<-G> I<file>

True if file exists and its group matches the effective
group ID of this process.

=end _NOT_IMPLEMENTED_

=item B<-R> I<file>

True if file exists and is readable by the real user/group.

=item B<-S> I<file>

True if file exists and is a socket.

=item B<-T> I<file>

True if file exists and is a text file.

=item B<-W> I<file>

True if file exists and is writable by the real user/group.  True
indicates only that the write flag is on.  The file is not writable on
a read-only file system even if this test indicates true.

=item B<-X> I<file>

True if file exists and is executable by the real user/group.
True indicates only that the execute flag is on.  If file is a
directory, true indicates that file can be searched.

=item I<file1> B<-nt> I<file2>

True if I<file1> exists and is newer than I<file2>.

=item I<file1> B<-ot> I<file2>

True if I<file1> exists and is older than I<file2>.

=begin _NOT_IMPLEMENTED_

=item I<file1> B<-ef> I<file2>

True if I<file1> and I<file2> exist and refer to the same file.

=end _NOT_IMPLEMENTED_

=item I<string>

True if I<string> is not the null string.

=item I<s1> = I<s2>

True if the strings I<s1> and I<s2> are identical.

=item I<s1> != I<s2>

True if the strings I<s1> and I<s2> are not identical.

=item I<s1> < I<s2>

True if string I<s1> comes before I<s2> based on the ASCII value
of their characters.

=item I<s1> > I<s2>

True if string I<s1> comes after I<s2> based on the ASCII value
of their characters.

=item I<s1>

True if I<s1> is not the null string.

=item I<n1> B<-eq> I<n2>

True if the integers I<n1> and I<n2> are algebraically equal.

=item I<n1> B<-ne> I<n2>

True if the integers I<n1> and I<n2> are not algebraically equal.

=item I<n1> B<-gt> I<n2>

True if the integer I<n1> is algebraically greater than the
integer I<n2>.

=item I<n1> B<-ge> I<n2>

True if the integer I<n1> is algebraically greater than or
equal to the integer I<n2>.

=item I<n1> B<-lt> I<n2>

True if the integer I<n1> is algebraically less than the integer I<n2>.

=item I<n1> B<-le> I<n2>

True if the integer I<n1> is algebraically less than or equal
to the integer I<n2>.

=back

These primaries can be combined with the following operators:

=over 14

=item ! I<expression>

True if I<expression> is false.

=item I<expression1> B<-a> I<expression2>

True if both I<expression1> and I<expression2> are true.

=item I<expression1> B<-o> I<expression2>

True if either I<expression1> or I<expression2> are true.

=item (I<expression>)

True if I<expression> is true (uses parentheses for grouping).

=back

Note that the B<-a> operator has higher precedence than the B<-o> operator.
Notice also that all the operators and flags are separate arguments to test.

=head1 RETURN VALUES

The test utility exits with one of the following values:

=over 8

=item $return_val == 0

Expression evaluated to true.

=item $return_val == 1

Expression evaluated to false or expression was missing.

=item $return_val > 1

An error occurred.

=back

=head1 CAVEATS

Command line arguments like parentheses and arithmetic operators
(e.g.: '(', ')', '!', '>', '<', etc.) I<may> be meaningful to the
command-line interpreter (shell) and therefore I<may> need to be
escaped from any special shell interpretation.

=head1 SEE ALSO

L<sh>, L<find>

=cut


