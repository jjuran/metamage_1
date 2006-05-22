#!/usr/bin/perl
#
# time - report the time running a sub-process

use Benchmark;

$t0 = new Benchmark;
$rc = system(@ARGV);
$t1 = new Benchmark;
$td = timediff($t1,$t0);
($real, $child_user, $child_system) = @$td[0,3,4];

# I'm pretty sure this is POSIX format
printf STDERR "\nreal %.2f\nuser %.2f\nsys  %.2f\n",
              $real, $child_user, $child_system;

$rc &= 0xffff;
if ($rc == 0xff00) { exit 127; } else { exit ($rc >> 8); }

__END__

=pod

=head1 NAME

time - times the execution of a command

=head1 SYNOPSIS

time command [argument ...]

=head1 DESCRIPTION

The B<time> command prints to standard error the elapsed time during
the execution of a command, the time spent in the system, and the time
spent in execution of the command.

=head1 BUGS

B<time> has a resolution of seconds for real elapsed time.  The accuracy
of B<time> is suspect, especially for small time measurements.  Some
implementations return 126 if command is found but cannot be
invoked.

=head1 EXIT STATUS

If the command is successfully invoked then B<time> returns
the exit status of the command.  Otherwise it returns 127.

=head1 COPYRIGHT and LICENSE

This program is copyright by dkulp 1999.

This program is free and open software. You may use, copy, modify, distribute
and sell this program (and any modified variants) in any way you wish,
provided you do not restrict others to do the same.

=cut
