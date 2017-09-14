#! /opt/perl5/bin/perl

use ExtUtils::testlib;
use Mytest3;

use File::Spec;

my $startX = 83;
my $startY = 57;
my $endX = 211;
my $endY = 234;
my $map = 'hugel.fld2';

my $filename = File::Spec->catfile('maps', $map);
my $fieldData;
if (open($f, "<", $filename)) {
	binmode($f);
	local($/);
	$fieldData = <$f>;
	close($f);
} else {
	die "Cannot open $filename for reading.";
}

my ($width, $height) = unpack("v v", substr($fieldData, 0, 4, ''));

my $result = Mytest3::do_path($fieldData, $width, $height, $startX, $startY, $endX, $endY);

my $distmap = {};

$distmap->{gg} = Mytest3::makeDistMap($fieldData, $width, $height);

if (open($f, ">", 'hugel.dist')) {
	binmode $f;
	print $f pack("a2 v1", 'V#', 4);
	print $f pack("v v", $width, $height);
	print $f $distmap->{gg};
	close $f;
} else {
	print "Could not open distfile.\n";
}

print "Result is $result.\n";
