#!/usr/bin/perl

use common::sense;
use Data::Dumper;
use List::Util qw(reduce);

my ($nodes, $adeg) = @ARGV;

die "Usage: ./makegraph.pl num_nodes avg_deg." unless defined $nodes && defined $adeg;

srand(42);

my @graph;
my $m = 0;

my $th = $nodes/$adeg * 2;

for (my $i = 0; $i < $nodes; $i++) {
	$graph[$i] = {};
}

for (my $i = 0; $i < $nodes; $i++) {
	for (my $j = int(rand($th)); $j < $nodes; $j += 1+int(rand($th))) {
		if ($i != $j) {
			my $cost = int(rand(420)) + 1;
			if (not defined $graph[$i]->{$j}) { $m += 2; }
			$graph[$i]->{$j} = $cost;
			$graph[$j]->{$i} = $cost;
		}
	}
}

print "$nodes $m\n";
for (my $i = 0; $i < $nodes; $i++) {
	my $sum = scalar keys %{$graph[$i]};
	print "$sum ".join(" ", map { "$_ $graph[$i]{$_}" } keys %{$graph[$i]})."\n";
}
