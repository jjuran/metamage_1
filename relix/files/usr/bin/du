#!/usr/bin/perl -w
#
# Perl Power Tools - du
# Greg Hewgill <greg@hewgill.com> 1999-03-07

use Getopt::Std;
use strict;

use vars qw($opt_H $opt_L $opt_P $opt_a $opt_c $opt_k $opt_l $opt_r $opt_s $opt_x);
use vars qw(%inodes $depth $blocksize $grandtotal $filesystem $dirsep);

%inodes = ();      # A record of where we've been
$depth = 0;        # Current recursion depth
$blocksize = 512;  # Default block size
$grandtotal = 0;   # Total of all command line argument totals
$filesystem = 0;   # Current file system (used for -x processing)
$dirsep = '/';     # Assume '/' is the directory separator

$blocksize = $ENV{BLOCKSIZE} if $ENV{BLOCKSIZE}; # use environment if present

# Process command line options
getopts('HLPacklrsx') or usage_exit(1);

$blocksize = 1024 if $opt_k;

if ($opt_a && $opt_s) {
  print STDERR "$0: cannot both summarize and show all entries\n";
  usage_exit(1);
}

# Default to traversing current directory if no files specified
unless (@ARGV) {
  push @ARGV, '.';
}

foreach (@ARGV) {
  %inodes = (); # clear this at the start of each traversal
  $grandtotal += traverse($_);
}
print "$grandtotal\ttotal\n" if $opt_c;

sub traverse {
  my $fn = $_[0];
  my $total = 0;
  local $depth = $depth + 1;
  my @s = ($opt_L || $opt_H && $depth == 1) ? stat $fn : lstat $fn;
  # If we can't stat the file, return silently
  return 0 unless @s;
  # Check for cross-filesystem traversals (-x option)
  if ($depth == 1) {
    $filesystem = $s[0];
  } elsif ($opt_x && $s[0] != $filesystem) {
    return 0;
  }
  if ($s[1] && !$opt_l) {
    # Check to see whether we've been here before (dev/inode is unique)
    if ($inodes{$s[0]}{$s[1]}) {
      return 0;
    }
    $inodes{$s[0]}{$s[1]} = 1;
  }
  # For regular files or links, tally them up
  unless (-d _) {
    $total = int(($s[7] + $blocksize-1) / $blocksize);
    print "$total\t$fn\n" if $opt_a || $depth == 1;
    return $total;
  }
  # Do recursion
  if (opendir(DIR, $fn)) {
    foreach (readdir(DIR)) {
      my $subdir = ($fn =~ /$dirsep$/o) ? "$fn$_" : "$fn$dirsep$_";
      # Don't try to traverse '.' or '..'
      $total += traverse($subdir) unless /^\.{1,2}$/;
    }
    closedir DIR;
    print "$total\t$fn\n" unless $opt_s;
  } else {
    print STDERR "$0: could not read directory $fn: $!\n";
  }
  print "$total\t$fn\n" if $opt_s && $depth == 1;
  return $total;
}

sub usage_exit {
  print STDERR "Usage: $0 [-H | -L | -P] [-a | -s] [-cklrx] [file ...]\n";
  exit(@_);
}

=head1 NAME

B<du> - display disk usage statistics

=head1 SYNOPSIS

B<du> [B<-H> | B<-L> | B<-P>] [B<-a> | B<-s>] [B<-cklrx>] [I<file> ...]

=head1 DESCRIPTION

The B<du> utility displays the file system block usage for each file argument
and for each directory in the file hierarchy rooted in each directory
argument. If no file is specified, the block usage of
the hierarchy rooted in the current directory is displayed.

The options are as follows:

B<-H>      Symbolic links on the command line are followed.  (Symbolic links
encountered in the tree traversal are not followed.)

B<-L>      All symbolic links are followed.

B<-P>      No symbolic links are followed.

B<-a>      Display an entry for each file in the file hierarchy.

B<-k>      By default, du displays the number of blocks as the number of
512-byte blocks. If the B<-k> flag is
specified, the number displayed is the number of 1024-byte
blocks.  Partial numbers of blocks are rounded up.

B<-c>      Display the grand total after all the arguments have been processed.

B<-l>      Count the size of all files, even if they have appeared already 
in another hard link.

B<-s>      Display only the grand total for the specified files.

B<-r>      Generate messages about directories that cannot be read, files
that cannot be opened, and so on.  This is the default case.
This option exists solely for conformance with X/Open Portability
Guide Issue 4 (``XPG4'').

B<-x>      Filesystem mount points are not traversed.

B<du> counts the storage used by symbolic links and not the files they
reference unless the B<-H> or B<-L> option is specified.  If either the B<-H>
or B<-L> options are specified, storage used by any symbolic links which are
followed is not counted or displayed.

Files having multiple hard links are counted (and displayed) a single
time per B<du> execution.

=head1 ENVIRONMENT

If the environment variable BLOCKSIZE is set, and the B<-k> option is not
specified, the block counts will be displayed in units of that size block.

=head1 BUGS

The directory separator is assumed to be '/'.

The number of blocks reported is based on the size of the file. This may
or may not reflect the actual number of blocks allocated by the file system
for the file.

B<du> will skip files it shouldn't if the underlying file system does
not support inodes, and does not always place 0 in the inode field of a
stat(2) call. In this case B<-l> can be specified to skip the inode check
and always list all files.

=head1 HISTORY

A B<du> command appeared in Version 6 AT&T UNIX.

=head1 AUTHOR

Greg Hewgill <greg@hewgill.com> 1999-03-07

=head1 COPYRIGHT and LICENSE

This program is Copyright (c) by Greg Hewgill 1999.

This program is free and open software. You may use, copy, modify,
distribute, and sell this program (and any modified variants) in any
way you wish, provided you do not restrict others from doing the same.
