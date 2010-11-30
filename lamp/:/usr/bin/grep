#!/usr/bin/perl

###
### DON'T FORGET TO UPDATE THE POD IF YOU ADD TO THE HISTORY!!!
###

# tcgrep: tom christiansen's rewrite of grep
# v1.0: Thu Sep 30 16:24:43 MDT 1993
# v1.1: Fri Oct  1 08:33:43 MDT 1993
#
# Revision by Greg Bacon <gbacon@cs.uah.edu>
# Fixed up highlighting for those of us trapped in terminfo
# implemented -f
# v1.2: Fri Jul 26 13:37:02 CDT 1996
#
# Revision by Greg Bacon <gbacon@cs.uah.edu>
# Avoid super-inefficient matching (almost twice as fast! :-)
# v1.3: Sat Aug 30 14:21:47 CDT 1997
#
# Revision by Paul Grassie <grassie@worldnet.att.net>
# Removed vestiges of Perl4, made strict
# v1.4: Mon May 18 16:17:48 EDT 1998
#
# Revision by Greg Bacon <gbacon@cs.uah.edu>
# Add fgrep functionality for PPT
# v1.5: Mon Mar  8 12:05:29 CST 1999
#
# Revision by Bill Benedetto <bbenedetto@goodyear.com>
# Added CLOSE after each file is done to reset counters
# v1.6: Sat Mar 20 23:05:35 1999
#
# Revision by Paul Johnson <paul@pjcj.net>
# Add support for bzip2 and zip files
# File names are printed by default when using -r and supplying no directory
# v1.7: Thu May 27 00:25:30 BST 1999

use strict;
				  # globals
use vars qw($Me $Errors $Grand_Total $Mult %Compress $Matches);

my ($matcher, $opt);              # matcher - anon. sub to check for matches
				  # opt - ref to hash w/ command line options

init();                           # initialize globals

($opt, $matcher) = parse_args();  # get command line options and patterns

matchfile($opt, $matcher, @ARGV); # process files

exit(2) if $Errors;
exit(0) if $Grand_Total;
exit(1);

###################################

sub init {
    ($Me = $0) =~ s!.*/!!;        # get basename of program, "tcgrep"
    $Errors = $Grand_Total = 0;   # initialize global counters
    $Mult = "";                   # flag for multiple files in @ARGV
    $| = 1;                       # autoflush output

    %Compress = (                 # file extensions and program names
	z   => 'zcat <',          # for uncompressing
	gz  => 'zcat <',
	Z   => 'zcat <',
	bz2 => 'bzcat <',
	zip => 'unzip -c',
    );
}

###################################

sub usage {
	die <<EOF
usage: $Me [flags] [files]

Standard grep options:
	i   case insensitive
	n   number lines
	c   give count of lines matching
	C   ditto, but >1 match per line possible
	w   word boundaries only
	s   silent mode
	x   exact matches only
	v   invert search sense (lines that DON'T match)
	h   hide filenames
	e   expression (for exprs beginning with -)
	f   file with expressions
	l   list filenames matching
	F   search for fixed strings (disable regular expressions)

Specials:
	1   1 match per file
	H   highlight matches
	u   underline matches
	r   recursive on directories or dot if none
	t   process directories in `ls -t` order
	p   paragraph mode (default: line mode)
	P   ditto, but specify separator, e.g. -P '%%\\n'
	a   all files, not just plain text files
	q   quiet about failed file and dir opens
	T   trace files as opened

May use a TCGREP environment variable to set default options.
EOF
}

###################################

sub parse_args {
    use Getopt::Std;

    my ($optstring, $zeros, $nulls, %opt, $pattern, @patterns, $match_code);
    my ($SO, $SE);

    if ($_ = $ENV{TCGREP}) {      # get envariable TCGREP
	s/^([^\-])/-$1/;          # add leading - if missing
	unshift(@ARGV, $_);       # add TCGREP opt string to @ARGV
    }

    $optstring = "incCwsxvhe:f:l1HurtpP:aqTF";

    $zeros = 'inCwxvhelut';       # options to init to 0 (prevent warnings)
    $nulls = 'pP';                # options to init to "" (prevent warnings)

    @opt{ split //, $zeros } = ( 0 )  x length($zeros);
    @opt{ split //, $nulls } = ( '' ) x length($nulls);

    getopts($optstring, \%opt)		or usage();

    my $no_re = $opt{F} || ($Me =~ /\bfgrep\b/);

    if ($opt{f}) {                # -f patfile
	open(PATFILE, $opt{f})		or die qq($Me: Can't open '$opt{f}': $!);

				  # make sure each pattern in file is valid
	while ( defined($pattern = <PATFILE>) ) {
	    chomp $pattern;
            unless ($no_re) {
	        eval { 'foo' =~ /$pattern/, 1 } or
		    die "$Me: $opt{f}:$.: bad pattern: $@";
            }
	    push @patterns, $pattern;
	}
	close PATFILE;
    }
    else {                        # make sure pattern is valid
	$pattern = $opt{e} || shift(@ARGV) || usage();
        unless ($no_re) {
	    eval { 'foo' =~ /$pattern/, 1 } or
	        die "$Me: bad pattern: $@";
        }
	@patterns = ($pattern);
    }

    if ($no_re) {
        for (@patterns) {
            # XXX: quotemeta?
            s/(\W)/\\$1/g;
        }
    }

    # mumble mumble DeMorgan mumble mumble
    if ($opt{v}) {
        @patterns = join '|', map "(?:$_)", @patterns;
    }

    if ($opt{H} || $opt{u}) {     # highlight or underline
	my $term = $ENV{TERM} || 'vt100';
	my $terminal;

	eval {                    # try to look up escapes for stand-out
	    require POSIX;        # or underline via Term::Cap
	    use Term::Cap;

	    my $termios = POSIX::Termios->new();
	    $termios->getattr;
	    my $ospeed = $termios->getospeed;

	    $terminal = Tgetent Term::Cap { TERM=>undef, OSPEED=>$ospeed }
	};

	unless ($@) {             # if successful, get escapes for either
	    local $^W = 0;        # stand-out (-H) or underlined (-u)
	    ($SO, $SE) = $opt{H}
		? ($terminal->Tputs('so'), $terminal->Tputs('se'))
		: ($terminal->Tputs('us'), $terminal->Tputs('ue'));
	}
	else {                    # if use of Term::Cap fails,
	    ($SO, $SE) = $opt{H}  # use tput command to get escapes
		? (`tput -T $term smso`, `tput -T $term rmso`)
		: (`tput -T $term smul`, `tput -T $term rmul`)
	}
    }

    if ($opt{i}) {
	@patterns = map {"(?i)$_"} @patterns;
    }

    if ($opt{p} || $opt{P}) {
	@patterns = map {"(?m)$_"} @patterns;
    }

    $opt{p}   && ($/ = '');
    $opt{P}   && ($/ = eval(qq("$opt{P}")));     # for -P '%%\n'
    $opt{w}   && (@patterns = map {'\b' . $_ . '\b'} @patterns);
    $opt{'x'} && (@patterns = map {"^$_\$"} @patterns);
    $Mult = 1 if ($opt{r} || @ARGV > 1 || @ARGV > 0 && -d $ARGV[0]) && !$opt{h};
    $opt{1}   += $opt{l};                   # that's a one and an ell
    $opt{H}   += $opt{u};
    $opt{c}   += $opt{C};
    $opt{'s'} += $opt{c};
    $opt{1}   += $opt{'s'} && !$opt{c};     # that's a one

    @ARGV = ($opt{r} ? '.' : '-') unless @ARGV;
    $opt{r} = 1 if !$opt{r} && grep(-d, @ARGV) == @ARGV;

    $match_code  = '';
    $match_code .= 'study;' if @patterns > 5; # might speed things up a bit

    foreach (@patterns) { s(/)(\\/)g }

    if ($opt{H}) {
	foreach $pattern (@patterns) {
	    $match_code .= "\$Matches += s/($pattern)/${SO}\$1${SE}/g;";
	}
    }
    elsif ($opt{v}) {
	foreach $pattern (@patterns) {
	    $match_code .= "\$Matches += !/$pattern/;";
	}
    }
    elsif ($opt{C}) {
	foreach $pattern (@patterns) {
	    $match_code .= "\$Matches++ while /$pattern/g;";
	}
    }
    else {
	foreach $pattern (@patterns) {
	    $match_code .= "\$Matches++ if /$pattern/;";
	}
    }

    $matcher = eval "sub { $match_code }";
    die if $@;

    return (\%opt, $matcher);
}

###################################

sub matchfile {
    $opt = shift;                 # reference to option hash
    $matcher = shift;             # reference to matching sub

    my ($file, @list, $total, $name);
    local($_);
    $total = 0;

FILE: while (defined ($file = shift(@_))) {

	if (-d $file) {
	    if (-l $file && @ARGV != 1) {
		warn "$Me: \"$file\" is a symlink to a directory\n"
		    if $opt->{T};
		next FILE;

	    }
	    if (!$opt->{r}) {
		warn "$Me: \"$file\" is a directory, but no -r given\n"
		    if $opt->{T};
		next FILE;
	    }
	    unless (opendir(DIR, $file)) {
		unless ($opt->{'q'}) {
		    warn "$Me: can't opendir $file: $!\n";
		    $Errors++;
		}
		next FILE;
	    }
	    @list = ();
	    for (readdir(DIR)) {
		push(@list, "$file/$_") unless /^\.{1,2}$/;
	    }
	    closedir(DIR);
	    if ($opt->{t}) {
		my (@dates);
		for (@list) { push(@dates, -M) }
		@list = @list[sort { $dates[$a] <=> $dates[$b] } 0..$#dates];
	    }
	    else {
		@list = sort @list;
	    }
	    matchfile($opt, $matcher, @list);    # process files
	    next FILE;
	}

	if ($file eq '-') {
	    warn "$Me: reading from stdin\n" if -t STDIN && !$opt->{'q'};
	    $name = '<STDIN>';
	}
	else {
	    $name = $file;
	    unless (-e $file) {
		warn qq($Me: file "$file" does not exist\n) unless $opt->{'q'};
		$Errors++;
		next FILE;
	    }
	    unless (-f $file || $opt->{a}) {
		warn qq($Me: skipping non-plain file "$file"\n) if $opt->{T};
		next FILE;
	    }

	    my ($ext) = $file =~ /\.([^.]+)$/;
	    if (defined $ext && exists $Compress{$ext}) {
		$file = "$Compress{$ext} $file |";
	    }
	    elsif (! (-T $file  || $opt->{a})) {
		warn qq($Me: skipping binary file "$file"\n) if $opt->{T};
		next FILE;
	    }
	}

	warn "$Me: checking $file\n" if $opt->{T};

	unless (open(FILE, $file)) {
	    unless ($opt->{'q'}) {
		warn "$Me: $file: $!\n";
		$Errors++;
	    }
	    next FILE;
	}

	$total = 0;

	$Matches = 0;

LINE:  while (<FILE>) {
	    $Matches = 0;

	    ##############
	    &{$matcher}();        # do it! (check for matches)
	    ##############

	    next LINE unless $Matches;

	    $total += $Matches;

	    if ($opt->{p} || $opt->{P}) {
		s/\n{2,}$/\n/ if $opt->{p};
		chomp         if $opt->{P};
	    }

	    print("$name\n"), next FILE if $opt->{l};

	    $opt->{'s'} || print $Mult && "$name:",
		$opt->{n} ? "$.:" : "",
		$_,
		($opt->{p} || $opt->{P}) && ('-' x 20) . "\n";

	    next FILE if $opt->{1};                 # that's a one
	}
    }
    continue {
	print $Mult && "$name:", $total, "\n" if $opt->{c};
	close FILE;
    }
    $Grand_Total += $total;
}

__END__

=pod

=head1 NAME

B<tcgrep> - search for regular expressions and print

=head1 SYNOPSIS

B<tcgrep> [ B<-[incCwsxvhlF1HurtpaqT]> ] [ B<-e> I<pattern> ]
[ B<-f> I<pattern-file> ] [ B<-P> I<sep> ] [ I<pattern> ] [ I<files> ... ]

=head1 DESCRIPTION

B<tcgrep> searches for lines (or, optionally, paragraphs) in files
that satisfy the criteria specified by the user-supplied patterns.
Because B<tcgrep> is a Perl program, the user has full access to
Perl's rich regular expression engine.  See L<perlre>.

The first argument after the options (assuming the user did not specify
the B<-e> option or the B<-f> option) is taken as I<pattern>.
If the user does not supply a list of file or directory names to
search, B<tcgrep> will attempt to search its standard input.

With no arguments, B<tcgrep> will output its option list and exit.

=head1 OPTIONS

B<tcgrep> accepts these options:

=over 4

=item B<-1>

Allow at most one match per file.

=item B<-a>

Search all files.  The default is to only search plain text files
and compressed files.

=item B<-C>

Output the count of the matching lines or paragraphs.  This is similar
to the B<-c> option (in fact, it implies the B<-c> option), except more
than one match is possible in each line or paragraph.

=item B<-c>

Output the count of the matching lines or paragraphs.

=item B<-e> I<pattern>

Treat I<pattern> as a pattern.  This option is most useful when
I<pattern> starts with a C<-> and the user wants to avoid confusing
the option parser.

The B<-f> option supercedes the B<-e> option.

=item B<-F>

B<fgrep> mode.  Disable regular expressions and perform Boyer-Moore
searches.  (Whether it lives up to the 'f' in B<fgrep> is another
issue).

=item B<-f> I<pattern-file>

Treat I<pattern-file> as a newline-separated list of patterns to use
as search criteria.

the B<-f> option supercedes the B<-e> option.

=item B<-H>

Highlight matches.  This option causes B<tcgrep> to attempt to use
your terminal's stand-out (emboldening) functionality to highlight
those portions of each matching line or paragraph that actually
triggered the match.  This feature is very similar to the way the
less(1) pager highlights matches.  See also B<-u>.

=item B<-h>

Hide filenames.  Only print matching lines or paragraphs.

=item B<-i>

Ignore case while matching.  This means, for example, that the pattern
C<unix> would match C<unix> as well as C<UniX> (plus the other fourteen
possible capitalizations).  This corresponds to the C</i> Perl regular
expression switch.  See L<perlre>.

=item B<-l>

List files containing matches.  This option tells B<tcgrep> not to
print any matches but only the names of the files containing matches.
This option implies the B<-1> option.

=item B<-n>

Number lines or paragraphs.  Before outputting a given match, B<tcgrep>
will first output its line or paragraph number corresponding to the
value of the Perl magic scalar $. (whose documentation is in L<perlvar>).

=item B<-P> I<sep>

Put B<tcgrep> in paragraph mode, and use I<sep> as the paragraph
separator.  This is implemented by assigning I<sep> to Perl's magic
$/ scalar.  See L<perlvar>.

=item B<-p>

Paragraph mode.  This causes B<tcgrep> to set Perl's magic $/ to C<''>.
(Note that the default is to process files in line mode.)  See L<perlvar>.

=item B<-q>

Quiet mode.  Suppress diagnostic messages to the standard error.  See
B<-s>.

=item B<-r>

Recursively scan directories.  This option causes B<tcgrep> to
descend directories in a left-first, depth-first manner and search
for matches in the files of each directory it encounters.  The
presence of B<-r> implies a file argument of F<.>, the current
directory, if the user does not provide filenames on the command line.
See L<"EXAMPLES">.

=item B<-s>

Silent mode.  Do not write to the standard output.  This option would
be useful from a shell script, for example, if you are only interested
in whether or not there exists a match for a pattern.  See also B<-q>.

=item B<-T>

Trace files as processed.  This causes B<tcgrep> to send diagnostic
messages to the standard error when skipping symbolic links to directories,
when skipping directories because the user did not give the B<-r> switch,
when skipping non-plain files (see L<perlfunc/-f>),
when skipping non-text files (see L<perlfunc/-T>), and
when opening a file for searching

=item B<-t>

Process directories in C<`ls -t`> order.  Search the files in each
directory starting with the most recently modified and ending with
the least recently modified.

=item B<-u>

Underline matches.  This option causes B<tcgrep> to attempt to use
your terminal's underline functionality to underline those portions of
each matching line or paragraph that actually triggered the match.
See also B<-H>.

=item B<-v>

Invert the sense of the match, i.e. print those lines or paragraphs
that do B<not> match.  When using this option in conjunction with B<-f>,
keep in mind that the entire set of patterns are grouped together in
one pattern for the purposes of negation.  See L<"EXAMPLES">.

=item B<-w>

Matches must start and end at word boundaries.  This is currently
implemented by surrounding each pattern with a pair of C<\b>, the
Perl regular expression word boundary metasequence.  See L<perlre>
for the precise definition of C<\b>.

=item B<-x>

Exact matches only.  The pattern must match the entire line or paragraph.

=back

=head1 ENVIRONMENT

The user's TCGREP environment variable is taken as the default set of
options to B<tcgrep>.

=head1 EXAMPLES

Search all files under F</etc/init.d> for a particular pattern:

    % tcgrep -r tcgrep /etc/init.d

Use of B<-v> and B<-f> options in conjunction with one another:

    % cat fruits
    pomme
    banane
    poire
    % cat pats
    pomme
    poire
    % tcgrep -vf pats fruits
    banane

=head1 TODO

=over 4

=item *

Add more cool examples. :-)

=item *

Perhaps allow the user to provide an exclusion pattern for skipping over
files whose names match the pattern.

=back

=head1 REVISION HISTORY

    tcgrep: tom christiansen's rewrite of grep
    v1.0: Thu Sep 30 16:24:43 MDT 1993
    v1.1: Fri Oct  1 08:33:43 MDT 1993

    Revision by Greg Bacon <gbacon@cs.uah.edu>
    Fixed up highlighting for those of us trapped in terminfo
    implemented -f
    v1.2: Fri Jul 26 13:37:02 CDT 1996

    Revision by Greg Bacon <gbacon@cs.uah.edu>
    Avoid super-inefficient matching (almost twice as fast! :-)
    v1.3: Sat Aug 30 14:21:47 CDT 1997

    Revision by Paul Grassie <grassie@worldnet.att.net>
    Removed vestiges of Perl4, made strict
    v1.4: Mon May 18 16:17:48 EDT 1998

    Revision by Greg Bacon <gbacon@cs.uah.edu>
    Add fgrep functionality for PPT
    v1.5: Mon Mar  8 12:05:29 CST 1999

    Revision by Bill Benedetto <bbenedetto@goodyear.com>
    Added CLOSE after each file is done to reset counters
    v1.6: Sat Mar 20 23:05:35 1999

    Revision by Paul Johnson <paul@pjcj.net>
    Add support for bzip2 and zip files
    File names are printed by default when using -r and supplying no directory
    v1.7: Thu May 27 00:25:30 BST 1999

=head1 AUTHOR

B<tcgrep> was written by Tom Christiansen with updates by Greg Bacon
and Paul Grassie.

=head1 COPYRIGHT and LICENSE

Copyright (c) 1993-1999. Tom Christiansen.

This program is free and open software. You may use, copy, modify, distribute,
and sell this program (and any modified variants) in any way you wish,
provided you do not restrict others from doing the same.

=cut
