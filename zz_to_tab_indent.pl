#!/usr/bin/env perl
use strict;
use warnings;
use File::Find;
use File::Copy qw(move);

# Configuration
my @exts = ('.c', '.h');    # file extensions to process
my $inplace_backup = 0;     # set to 1 to keep .bak backups, 0 for no backup
my $only_commented = 0;

main();

sub main {
    my $start = '.';             # recursive from current directory
    find({ wanted => \&wanted, no_chdir => 1 }, $start);
    $only_commented = 1;
    find({ wanted => \&wanted, no_chdir => 1 }, $start);
}

sub wanted {
    return unless -f $File::Find::name;               # only regular files
    return unless _has_target_ext($File::Find::name); # check extension
    process_file($File::Find::name);
}

sub _has_target_ext {
    my ($path) = @_;
    foreach my $e (@exts) {
        return 1 if $path =~ /\Q$e\E$/;
    }
    return 0;
}

sub process_file {
    my ($path) = @_;

    open my $in, '<', $path or do {
        warn "Cannot open '$path' for reading: $!\n";
        return;
    };

    my $tmp = "$path.tmp";
    open my $out, '>', $tmp or do {
        warn "Cannot open temporary file '$tmp' for writing: $!\n";
        close $in;
        return;
    };

    while (defined(my $line = <$in>)) {
		if($only_commented) {
			$line = convert_leading_spaces_to_tabs($line);
			print $out $line;
		} else {
			$line = convert_commented_leading_spaces_to_tabs($line);
			print $out $line;
		}
    }

    close $in;
    close $out;

    if ($inplace_backup) {
        move($path, "$path.bak") or warn "Failed to move $path to .bak: $!\n";
        move($tmp, $path) or warn "Failed to move $tmp to $path: $!\n";
    } else {
        move($tmp, $path) or warn "Failed to replace $path with $tmp: $!\n";
    }
}

# Convert any number of leading groups of 4 spaces into tabs.
# Preserves existing leading tabs and any leftover spaces (less than 4).
sub convert_leading_spaces_to_tabs {
    my ($line) = @_;
    return $line unless defined $line;

    # capture leading run early
    return $line unless $line =~ /^([\t ]+)/;
    my $lead = $1;

    # compute original leading length near the start as requested
    my $orig_len = length($1 // '');

    # convert groups of 4 spaces to tabs within the leading run
    $lead =~ s/ {4}/\t/g;

    # safely extract the rest of the line after the original leading run
    my $line_len = length($line // '');
    my $rest = $line_len > $orig_len ? substr($line, $orig_len) : '';

    return $lead . $rest;
}

sub convert_commented_leading_spaces_to_tabs {
    my ($line) = @_;
    return $line unless defined $line;

    # capture leading run early
    return $line unless $line =~ /^\/\/([\t ]+)/;
    my $lead = $1;

    # compute original leading length near the start as requested
    my $orig_len = length($1 // '') + 2;

    # convert groups of 4 spaces to tabs within the leading run
    $lead =~ s/ {4}/\t/g;

    # safely extract the rest of the line after the original leading run
    my $line_len = length($line // '');
    my $rest = $line_len > $orig_len ? substr($line, $orig_len) : '';

    return "//" . $lead . $rest;
}
