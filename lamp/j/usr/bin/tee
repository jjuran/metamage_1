#!/usr/bin/perl
#
# tee clone that groks process tees (should work even with old perls)
# Tom Christiansen <tchrist@convex.com>
# 6 June 91

while ($ARGV[0] =~ /^-(.+)/ && (shift, ($_ = $1), 1)) {
    next if /^$/;
    s/i// && (++$ignore_ints, redo); 
    s/a// && (++$append,      redo);
    s/u// && (++$unbuffer,    redo);
    s/n// && (++$nostdout,    redo);
    die "usage tee [-aiun] [filenames] ...\n";
} 
if ($ignore_ints) {
    for $sig ('INT', 'TERM', 'HUP', 'QUIT') { $SIG{$sig} = 'IGNORE'; } 
}
$SIG{'PIPE'} = 'PLUMBER';
$mode = $append ? '>>' : '>';
$fh = 'FH000';
unless ($nostdout) { 
    %fh = ('STDOUT', 'standard output'); # always go to stdout
}
$| = 1 if $unbuffer;

for (@ARGV) {
    if (!open($fh, (/^[^>|]/ && $mode) . $_)) {
	warn "$0: cannot open $_: $!\n"; # like sun's; i prefer die
	$status++;
	next;
    }
    select((select($fh), $| = 1)[0]) if $unbuffer;
    $fh{$fh++} = $_;
} 
while (<STDIN>) {
    for $fh (keys %fh) {
	print $fh $_;
    } 
} 
for $fh (keys %fh) { 
    next if close($fh) || !defined $fh{$fh};
    warn "$0: couldn't close $fh{$fh}: $!\n";
    $status++;
}
exit $status;

sub PLUMBER {
    warn "$0: pipe to \"$fh{$fh}\" broke!\n";
    $status++;
    delete $fh{$fh};
} 
