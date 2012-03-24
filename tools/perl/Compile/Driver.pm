package Compile::Driver;

use Compile::Driver::Jobs;

use warnings;
use strict;


sub jobs_for
{
	my ( $module ) = @_;
	
	my @jobs;
	
	my $target = $module->target;
	
	my $product = $module->product_type;
	
	my $is_lib = $module->is_static_lib;
	my $is_exe = $module->is_executable;
	
	my $type = $is_lib ? "AR"
	         : $is_exe ? "LINK"
	         :           "";
	
	return if !$type;
	
	my @sources = $module->sources;
	
	my $name = $module->name;
	
	my @objs;
	
	foreach my $path ( @sources )
	{
		my $obj = Compile::Driver::Jobs::obj_pathname( $target, $name, $path );
		
		push @objs, $obj;
		
		my $compile =
		{
			TYPE => "CC",
			FROM => $module,
			PATH => $path,
			DEST => $obj,
		};
		
		push @jobs, $compile;
	}
	
	my $dest = $is_lib ? Compile::Driver::Jobs::lib_pathname( $target, $name )
	                   : Compile::Driver::Jobs::bin_pathname( $target, $name, $module->program_name );
	
	my $link =
	{
		TYPE => $type,
		FROM => $module,
		OBJS => \@objs,
		DEST => $dest,
	};
	
	$link->{PREQ} = $module->recursive_prerequisites  if $is_exe;
	
	push @jobs, $link;
	
	return @jobs;
}

1;

