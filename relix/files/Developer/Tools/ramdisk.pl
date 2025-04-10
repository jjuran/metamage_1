#!/usr/bin/perl -w

#use strict;

my $copier;

my @whitelist = qw( fenv.h fp.h vBasicOps.h vBigNum.h vBLAS.h vDSP.h vectorOps.h vfp.h );

# Types.h is included by SANE.h

my @blacklist =
qw
(
	CursorCtl.h Desk.h DriverSupport.h Errors.h ErrMgr.h Errors.h FragLoad.h
	FSpio.h GestaltEqu.h IntEnv.h Interrupts.h Kernel.h Keychain.h Language.h
	MacRuntime.h MC68000Test.h Memory.h MP.h MPWLibsDebug.h NullDef.h
	OpenTptAppleTalk.h OpenTptClient.h OpenTptCommon.h OpenTptConfig.h
	OpenTptDevLinks.h OpenTptGlobalNew.h OpenTptInternet.h OpenTptISDN.h
	OpenTptLinks.h OpenTptModule.h OpenTptPCISupport.h OpenTptSerial.h
	OpenTptXTI.h OSEvents.h OTDebug.h OTSharedLibs.h Picker.h PictUtil.h
	SeekDefs.h SIO.h SizeTDef.h SoundComponents.h SoundInput.h Speech.h Strings.h
	Unicode.h VaListTDef.h WCharTDef.h Windows.h
);

my %xfer =
(
	"Interfaces&Libraries" =>
	{
		"Interfaces" =>
		{
			"CIncludes" =>
			sub
			{
				my $want = /^[A-Z]/;
				my $x    = $_;
				
				!$want == not grep { $_ eq $x } $want ? @blacklist : @whitelist;
			},
			"." => [ map { $_ . "Includes"  } qw( CWANSI CWC R ) ]
		},
		"Libraries"  =>
		{
			"MW68KLibraries" => sub { /MPW/ || /SIOUX/ || /2i/ },
			"MWPPCLibraries" => sub { /MPW/ || /SIOUX/ },
			"Libraries" => sub { /^SIO/ || /^ToolLibs/ },
			"PPCLibraries" => sub { /^PPC/ || /^StdC/ || /^MrC/ },
			"." => [ "SharedLibraries" ]
		},
	},
	"MPW" =>
	{
		"Tools" => [ qw( FLEXlm license.dat InformDbgr MWC68K MWCPPC MWLink68K MWLinkPPC Rez ) ],
		"." =>
		[
			qw( Help MPW.Help StartupTS SysErrs.err ToolServer ),
			map { $_ . "Startup Items" } "", "TS "
		]
	},
);


sub read_file
{
	my ( $path ) = @_;
	
	open my $in, "<", $path or die "Can't open $path: $!\n";
	
	local $/;
	
	my $data = <$in>;
}

sub write_file
{
	my ( $path, $data ) = @_;
	
	open my $out, ">", $path or die "Can't open $path: $!\n";
	
	print $out $data;
	
	return;
}


sub transfer_one
{
	my ( $src, $dest ) = @_;
	
	if ( readlink $src  ||  -f $src )
	{
		print $copier "$src\0$dest\0\n" or die "Error writing to copier: $!\n";
	}
	else
	{
		transfer( "$src", "$dest" );
	}
}

sub transfer
{
	my ( $src, $dest, $xfer ) = @_;
	
	local $| = 1;
	
	my ( $basename ) = $src =~ m{ ( [^/]* ) $}x;
	
	print "\r";
	
	print "Copy $basename..." . (" " x (31 - length $basename));
	
	print $copier "\0$dest\0\n" or die "Error writing to copier: $!\n" unless $basename eq ".";
	
	if ( ref $xfer eq "HASH" )
	{
		foreach my $name ( keys %$xfer )
		{
			my $set = $xfer->{$name};
			
			-d "$src/$name" or die "Missing $src/$name\n";
			
			transfer( "$src/$name", "$dest/$name", $set );
		}
	}
	elsif ( ref $xfer eq "ARRAY" )
	{
		foreach my $name ( @$xfer )
		{
			transfer_one( "$src/$name", "$dest/$name" );
		}
	}
	else
	{
		my $filter = ref $xfer eq "CODE" ? $xfer : sub { undef };
		
		opendir my $dir, $src or die "Can't open $src: $!\n";
		
		foreach my $name ( readdir $dir )
		{
			next if $name eq ".";
			next if $name eq "..";
			next if $name eq ".DS_Store";
			
			next if grep { $filter->() } $name;
			
			transfer_one( "$src/$name", "$dest/$name" );
		}
	}
}

sub check
{
	my $ramdisk = `ram-disks -1`;
	
	if ( $ramdisk )
	{
		chomp $ramdisk;
		
		my $name = read_file( "$ramdisk/name" );
		
		chomp $name;
		
		print "RAM disk '$name' found at $ramdisk\n";
	}
}

sub auto
{
	my $ramdisk = `ram-disks -1`;
	
	if ( ! $ramdisk )
	{
		print "No RAM disk found.\n";
		
		return;
	}
	
	chomp $ramdisk;
	
	my $ram = "$ramdisk/mnt";
	
	my $name = read_file( "$ramdisk/name" );
	
	chomp $name;
	
	print "RAM disk '$name' found at $ramdisk\n";
	
	my $mpw = `mpw-select -p`;
	
	if ( !defined $mpw )
	{
		die "MPW not found.\n";
	}
	
	print "MPW dir: $mpw";  # $mpw ends with LF
	
	( my $dev_apps = $mpw ) =~ s{ / [^/]+ \n$}{}x;
	
	my $intfs_libs = "$dev_apps/Interfaces&Libraries";
	
	-d $intfs_libs or die "Interfaces&Libraries not found.\n";
	
	print "SDK dir: $intfs_libs\n";
	
	-d "$ram/tmp" or mkdir "$ram/tmp" or die "mkdir: $ram/tmp: $!\n";
	
	open $copier, "| copier" or die "Can't run the copier: $!\n";
	
	transfer( $dev_apps, "$ram/tmp/Applications", \%xfer );
	
	local $| = 1;
	
	print "\r";
	
	print "Finishing..." . (" " x 40);
	
	close $copier or die "Error closing the copier: $?\n";
	
	rename "$ram/tmp/Applications", "$ram/Applications" or die "Can't move Applications out of tmp: $!\n";
	
	print "\r";
	
=pod
	
	We need to have the MPW window open long enough for the Finder
	to inform the Desktop Manager of the new ToolServer installation.
	
	We used to do this by explicitly querying the Desktop Manager
	by way of the vfs mapping in MacRelix.  Though as the only client
	to use it, the cost of maintaining it was not well compensated.
	
	Now we run mpw-select, which specifically asks the Desktop Manager
	about ToolServer, but only if MPW_DIR is unset.  So in order to use
	it thus, we may need to unset MPW_DIR ourselves.  It's at least
	theoretically possible that mpw-select might fail when MPW_DIR is
	unset, so guard against an undefined value by substituting "".
	
=cut
	
	if ( exists $ENV{MPW_DIR} )
	{
		delete $ENV{MPW_DIR};
		
		$mpw = `mpw-select -p` || "";
	}
	
	system "open $ram/Applications/MPW";
	
	sleep 0.1 until (`mpw-select -p` || "") ne $mpw;
	
	my $script = 'tell app "Finder" to close the front window';
	
	system "osascript", "-e", $script, "-e", '""';
	
	print "\r";
	
	print "Done." . (" " x 50);
	print "\n";
	
	return;
}


my $command = shift;

$command = "" if !defined $command;

my %handler_for =
(
	""   => sub { check },
	auto => sub { auto  },
);

my $handler = $handler_for{$command};

$handler->() if defined $handler;
