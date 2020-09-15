#!/usr/bin/perl -w
# tr - translate or delete characters

use strict;

my(
    %opt,           # option hash
    $string1,	    # from set
    $string2,       # to set
);


END {
    close STDOUT || die "$0: can't close stdout: $!\n";
    $? = 1 if $? == 255;  # from die
} 

sub usage {
    warn "$0: @_\n" if @_;
    die "usage: $0 [-csd] string1 string2 [file ...]\n";
} 

@opt{ qw/c s d U C/ } = ('') x 5;

while ($ARGV[0] =~ s/^-(?=.)//) {
    for my $flag (split(//,$ARGV[0])) {
        usage("unknown flag: `$flag'") unless 'csdUC' =~ /\Q$flag/;
        warn "$0: `$flag' flag already set\n" if $opt{$flag};
        $opt{$flag} = $flag;
    } 
    shift;
}

usage("Not enough arguments") unless @ARGV > 1;

# remove spurious [...]
for (($string1, $string2) = splice(@ARGV, 0, 2)) {
    s/^\[(.*?)\]$/$1/;
} 

my $opts = join('', @opt{qw/c s d/});

# this is in an eval because tr/// needs to 
# see its parts at compile time
eval qq{
    while (<>) {
	tr[$string1][$string2]$opts;
	print;
    } 
};

die if $@;

exit (0); # XXX: Doen't detect failed opens

__END__

=head1 NAME

tr - translate or delete characters

=head1 SYNOPSIS

tr [ -cdsUC ] [ I<SEARCHLIST> [ I<REPLACEMENTLIST> ] ]

=head1 DESCRIPTION

The I<tr> program copies the standard input to the standard output
with substitution or deletion of selected characters.  Input characters
found in I<SEARCHLIST> are mapped into the corresponding characters of
I<REPLACEMENTLIST>. When I<REPLACEMENTLIST> is short it is padded to
the length of I<SEARCHLIST> by duplicating its last character.

Here are the options:

=over

=item c   

Complement the SEARCHLIST.

=item d   

Delete found but unreplaced characters.

=item s   

Squash duplicate replaced characters.

=item U   

Translate to/from UTF-8.

=item C   

Translate to/from 8-bit char (octet).

=back

In either string, the notation C<a-b> means a range of characters from
C<a> to C<b> in increasing ASCII order. Customary Perl escapes are
honored, such as C<\n> for newline, C<\012> for octal, and C<\x0A>
for hexadecimal codes.

If the B<-c> flag is specified, the SEARCHLIST character set is
complemented.  If the B<-d> flag is specified, any characters specified
by SEARCHLIST not found in REPLACEMENTLIST are deleted.  (Note that
this is slightly more flexible than the behavior of some tr programs,
which delete anything they find in the SEARCHLIST, period.)  If the B<-s>
flag is specified, sequences of characters that were transliterated to the
same character are squashed down to a single instance of the character.

If the B<-d> flag is used, the REPLACEMENTLIST is always interpreted
exactly as specified.  Otherwise, if the REPLACEMENTLIST is shorter
than the SEARCHLIST, the final character is replicated till it is long
enough.  If the REPLACEMENTLIST is empty, the SEARCHLIST is replicated.
This latter is useful for counting characters in a class or for squashing
character sequences in a class.

The first B<-U> or B<-C> flag applies to the left side of the translation.
The second one applies to the right side.  If present, these flags
override the current utf8 state.

=head1 EXAMPLES

The following command creates a list of all the words in F<file1>
one per line in F<file2>, where a word is taken to be a maximal string
of alphabetics.

    tr -cs A-Za-z "\n" <file1 >file2

The following command strips the 8th bit from an input file:

    tr "\200-\377" "\000-\177"

The following command translates Latin-1 to Unicode:

    tr -CU "\0-\xFF" "" 

The following command translates Unicode to Latin-1

    tr -UC "\0-\x{FF}" "" 

=head1 NOTE

This command is implemented using Perl's C<tr> operator.
See the documentation in L<perlop> for details on its
operation.

=head1 BUGS

There is no way to catch the file open error on ARGV handle
processing, so the exit status does not reflect file open
failures.

Not all systems have Unicode support yet, in which case the
B<-U> or B<-C> flags would cause a fatal error.

=head1 AUTHOR

Tom Christiansen, I<tchrist@perl.com>.

=head1 COPYRIGHT and LICENSE

This program is copyright (c) Tom Christiansen 1999.

This program is free and open software. You may use, modify, distribute,
and sell this program (and any modified variants) in any way you wish,
provided you do not restrict others from doing the same.
