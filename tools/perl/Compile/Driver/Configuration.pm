package Compile::Driver::Configuration;

use Compile::Driver::Catalog;
use Compile::Driver::Module;
use Compile::Driver::Platform;

use warnings FATAL => 'all';
use strict;


# Host config
my $is_osx = $^O eq "darwin";

my $osx_arch = $is_osx ? substr( `arch`, 0, -1 ) : "";

$osx_arch =~ s/^ i386 /x86/x;

my $is_mac = $is_osx;

my $build = "dbg";


my %category_of_spec = qw
(
	ppc  arch
	x86  arch
	arm  arch
	
	m32  width
	m64  width
	
	sym  build
	dbg  build
	opt  build
);

sub hosttype
{
	my $hosttype = substr( `sh -c 'echo \$HOSTTYPE'`, 0, -1 );
	
	if ( $osx_arch eq "x86"  &&  $hosttype eq "" )
	{
		# In Mac OS X 10.7, running `echo $HOSTTYPE` in the shell outputs
		# "x86_64", but `sh -c 'echo $HOSTYPE'` just emits a blank line.
		
		$hosttype = substr( `uname -m`, 0, -1 );
	}
	
	return $hosttype;
}

sub new
{
	my $class = shift;
	
	my @specs = @_;
	
	my %self;
	
	foreach my $spec ( @specs )
	{
		my $category = $category_of_spec{ $spec };
		
		if ( !exists $self{ $category } )
		{
			$self{ $category } = $spec;
		}
		elsif ( $self{ $category } ne $spec )
		{
			die "Can't set $category=$spec; $category is already $self{ $category }\n";
		}
	}
	
	if ( $is_mac )
	{
		$self{ arch } ||= $osx_arch;
		
		my $is_arm = $osx_arch eq "arm64";
		
		my $wants_64bit = $is_arm || hosttype() =~ /x86_64/;
		
		$self{width} ||= $wants_64bit ? "m64" : "m32";
		
		if ( $self{ width } eq "m64" )
		{
			$self{ arch } =~ s/^ ppc $/ppc64/x;
			$self{ arch } =~ s/^ x86 $/x86_64/x;
		}
		
		$self{ mac_runtime } = 'mach-o';
		
		$self{ mac_api } = 'carbon';
		
		$self{ mac } = 'mac';
	}
	
	$self{ unix } = "unix";
	
	$self{ build } = $build  if !exists $self{ build };
	
	return bless \%self, $class;
}

sub platform_mask
{
	my $self = shift;
	
	my %platform = %$self;
	
	delete $platform{ build };
	
	return Compile::Driver::Platform::mask_for_values( values %platform );
}

sub target
{
	my $self = shift;
	
	my $build = $self->{build};
	
	if ( !$is_mac )
	{
		return $build;
	}
	
	my ( $arch, $rt, $api ) = @{ $self }{ qw( arch mac_runtime mac_api ) };
	
	$rt  =~ s{^ (....) .* }{$1}x;
	$api =~ s{^ (....) .* }{$1}x;
	
	return "$arch-$rt-$api-$build";
}

my %module_cache;

sub get_module
{
	my $self = shift;
	
	my ( $name, $mandatory ) = @_;
	
	my $optional = !$mandatory  &&  ($name =~ m{^ _ }x  ||  $name =~ s{\?$}{});
	
	my $mask = $self->platform_mask;
	
	my $key = "$name $mask";
	
	return $module_cache{ $key }  if exists $module_cache{ $key };
	
	my $desc = Compile::Driver::Catalog::find_project( $name, $mask );
	
	if ( !$desc )
	{
		if ( $optional )
		{
			$desc = { NAME => $name, MEMO => { PREREQS => [] } };
		}
		else
		{
			die "No such project '$name'.  Run `make catalog` and try again.\n";
		}
	}
	
	return $module_cache{ $key } = Compile::Driver::Module::->new( $self, $desc );
}

sub conflicts_with
{
	my $self = shift;
	
	my ( $spec ) = @_;
	
	my $category = 'arch';
	
	my $value = $self->{ $category } or return 1;
	
	return $spec ne $value;
}

sub arch_option
{
	my $self = shift;
	
	my $arch = $self->{arch} or return;
	
	$arch =~ s/^ x86 $/i386/x;
	
	return (-arch => $arch);
}

sub is_carbon
{
	my $self = shift;
	
	return ($self->{ mac_api } || "") eq 'carbon';
}

sub is_apple_gcc
{
	my $self = shift;
	
	return ($self->{ mac_runtime } || "") eq "mach-o";
}

sub debugging
{
	my $self = shift;
	
	return $self->{ build } ne "opt";
}

sub symbolics
{
	my $self = shift;
	
	return $self->{ build } eq "sym";
}

1;
