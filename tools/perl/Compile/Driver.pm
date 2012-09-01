package Compile::Driver;

use Compile::Driver::Configuration;
use Compile::Driver::Job;
use Compile::Driver::Job::Compile;
use Compile::Driver::Job::Link::Archive;
use Compile::Driver::Job::Link::Binary;
use Compile::Driver::Module;
use Compile::Driver::Options;

use warnings;
use strict;


sub jobs_for
{
	my ( $module ) = @_;
	
	return if !$module->is_buildable;
	
	my @jobs;
	
	my $target = $module->target;
	
	my @sources = $module->sources;
	
	my $name = $module->name;
	
	my @objs;
	my @tool_objs;
	
	foreach my $path ( @sources )
	{
		my $obj = Compile::Driver::Job::obj_pathname( $target, $name, $path );
		
		if ( $module->source_is_tool( $path ) )
		{
			push @tool_objs, $obj;
		}
		else
		{
			push @objs, $obj;
		}
		
		my $compile = Compile::Driver::Job::Compile::->new
		(
			TYPE => "CC",
			FROM => $module,
			PATH => $path,
			DEST => $obj,
		);
		
		push @jobs, $compile;
	}
	
	my $link;
	
	if ( @objs  and  $module->is_static_lib  ||  $module->is_toolkit )
	{
		$link = Compile::Driver::Job::Link::Archive::->new
		(
			TYPE => "AR",
			FROM => $module,
			OBJS => \@objs,
			DEST => Compile::Driver::Job::lib_pathname( $target, $name ),
		);
	}
	elsif ( $module->is_executable )
	{
		$link = Compile::Driver::Job::Link::Binary::->new
		(
			TYPE => "LINK",
			FROM => $module,
			OBJS => \@objs,
			PREQ => $module->recursive_prerequisites,
			DEST => Compile::Driver::Job::bin_pathname( $target, $name, $module->program_name ),
		);
	}
	
	push @jobs, $link  if defined $link;
	
	my @prereqs = @{ $module->recursive_prerequisites };
	
	push @prereqs, $name;
	
	foreach my $obj ( @tool_objs )
	{
		my ( $tool_name ) = $obj =~ m{ / ( [^/]+ ) \. \w+ \.o }x;
		
		my $link = Compile::Driver::Job::Link::Binary::->new
		(
			TYPE => "LINK",
			PATH => $tool_name,
			FROM => $module,
			OBJS => [ $obj ],
			PREQ => \@prereqs,
			DEST => Compile::Driver::Job::bin_pathname( $target, $name, $tool_name ),
		);
		
		push @jobs, $link;
	}
	
	return @jobs;
}

sub main
{
	my @args = Compile::Driver::Options::set_options( @_ );
	
	my $configuration = Compile::Driver::Configuration::->new( Compile::Driver::Options::specs );
	
	if ( !Compile::Driver::Options::specs  &&  $configuration->{mac} )
	{
		my $dir = $Compile::Driver::Job::build_dir;
		
		my $target = $configuration->target;
		my $alias  = $configuration->{build};
		
		symlink $target, "$dir/$alias";
	}
	
	foreach my $name ( @args )
	{
		my $module = $configuration->get_module( $name, "[mandatory]" );
	}
	
	my $desc = { NAME => "[program args]", DATA => { use => [ @args ] } };
	
	my $module = Compile::Driver::Module::->new( $configuration, $desc );
	
	my @prereqs = @{ $module->recursive_prerequisites };
	
	pop @prereqs;
	
	my @jobs = map { Compile::Driver::jobs_for( $configuration->get_module( $_ ) ) } @prereqs;
	
	foreach my $job ( @jobs )
	{
		$job->perform;
	}
	
}

1;

