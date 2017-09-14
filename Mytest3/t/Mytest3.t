# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl Mytest3.t'

#########################

# change 'tests => 2' to 'tests => last_test_to_print';

use strict;
use warnings;

use Test::More tests => 2;
BEGIN { use_ok('Mytest3') };


my $fail = 0;
foreach my $constname (qw(
	CLOSED DIAGONAL NONE OPEN ORTOGONAL PATH)) {
  next if (eval "my \$a = $constname; 1");
  if ($@ =~ /^Your vendor has not defined Mytest3 macro $constname/) {
    print "# pass: $@";
  } else {
    print "# fail: $@";
    $fail = 1;
  }

}

ok( $fail == 0 , 'Constants' );
#########################

# Insert your test code below, the Test::More module is use()ed here so read
# its man page ( perldoc Test::More ) for help writing this test script.

