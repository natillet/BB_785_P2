#!/usr/bin/perl

use strict;
use warnings;

my $result;
my @results;
my $times = 0;
my $original_file_buffer;

open(FILE, "original_results.txt") or die $!;
$original_file_buffer = <FILE>;
close(FILE);

for (1 .. 5) {
	print "Iteration $_\n";
	my $new_file_buffer;
	my $cmd_out = `./project2`;
#	print "Result: $cmd_out";
	push(@results, $cmd_out);
	open(FILE, "results.txt") or die $!;
	$new_file_buffer = <FILE>;
	close(FILE);
	if ($original_file_buffer ne $new_file_buffer) {
		die "Results do not match!";
	}
}

foreach my $time (@results) {
	$time =~ /Execution duration in cycles \(rounded to the nearest 64 cycles\): (\d+)/;
	my $cycles = $1;
	print "$cycles\n";
	$times += $cycles;
}

print "Average: " . ($times/5) . "\n";

exit 0;
