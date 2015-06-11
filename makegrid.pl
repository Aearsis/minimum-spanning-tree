#!/usr/bin/perl

use common::sense;
use Data::Dumper;
use List::Util qw(reduce);

my ($w, $h) = @ARGV;

die "Usage: ./makegrid.pl width height." unless defined $w && defined $h;

srand(42);

my @graph;
my $m = 0;

my $nodes = $w * $h;

for (my $i = 0; $i < $nodes; $i++) {
	$graph[$i] = {};
}

for (my $i = 0; $i < $nodes; $i++) {
	if ($i % $w != $w - 1) {
		my $cost = int(rand(420)) + 1;
		$graph[$i]->{$i + 1} = $cost;
		$graph[$i + 1]->{$i} = $cost;
		$m += 2;
	}
	if ($i + $w < $nodes) {
		my $cost = int(rand(420)) + 1;
		$graph[$i]->{$i + $w} = $cost;
		$graph[$i + $w]->{$i} = $cost;
		$m += 2;
	}
}

print "$nodes $m\n";
for (my $i = 0; $i < $nodes; $i++) {
	my $sum = scalar keys %{$graph[$i]};
	print "$sum ".join(" ", map { "$_ $graph[$i]{$_}" } keys %{$graph[$i]})."\n";
}
