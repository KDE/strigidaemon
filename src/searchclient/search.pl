#! /usr/bin/perl
use strict;
use Socket;

my $query = join(" ", @ARGV);

# define the socket location
my $rendezvous = "$ENV{HOME}/.strigi/socket";
# 
socket(SOCK, PF_UNIX, SOCK_STREAM, 0)       || die "socket: $!";
select(SOCK);
$|=1;
select(STDOUT);
connect(SOCK, sockaddr_un($rendezvous))     || die "connect: $!";

# send the request
if (length($query)) {
    print SOCK "query\n$query\n\n";
} else {
    print SOCK "getStatus\n\n";
}
# read the result
my $line;
while (defined($line = <SOCK>)) {
    print $line;
}
close(SOCK) || die "close: $!";
