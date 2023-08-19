package Compile::Driver;

use Compile::Driver::Configuration;
use Compile::Driver::Job;
use Compile::Driver::Job::Compile;
use Compile::Driver::Job::Copy;
use Compile::Driver::Job::InfoPList;
use Compile::Driver::Job::Link::Archive;
use Compile::Driver::Job::Link::Binary;
use Compile::Driver::Job::Link::PkgInfo;
use Compile::Driver::Job::Rez;
use Compile::Driver::Module;
use Compile::Driver::Options;

use warnings FATAL => 'all';
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
	my $copy;
	
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
		my $prog = $module->program_name;
		my $path = Compile::Driver::Job::bin_pathname( $target, $name, $prog );
		
		if ( my $bundle_type = $module->bundle_type )
		{
			my $bundle = "$path.$bundle_type";
			
			push @jobs, Compile::Driver::Job::Link::PkgInfo::->new
			(
				TYPE => "BNDL",
				FROM => $module,
				DEST => "$bundle/Contents/PkgInfo",
			);
			
			if ( my $info_txt = $module->info_txt )
			{
				push @jobs, Compile::Driver::Job::InfoPList::->new
				(
					TYPE => "INFO",
					FROM => $module,
					ORIG => $info_txt,
					DEST => "$bundle/Contents/Info.plist",
				);
			}
			
			if ( my @rez = $module->rez_files )
			{
				push @jobs, Compile::Driver::Job::Rez::->new
				(
					TYPE => "REZ",
					FROM => $module,
					SRCS => \@rez,
					DEST => "$bundle/Contents/Resources/$prog.rsrc",
				);
			}
			
			foreach my $resource ( $module->resources )
			{
				my ( $basename ) = $resource =~ m{^ .* / ([^/]+) $}x;
				
				push @jobs, Compile::Driver::Job::Copy::->new
				(
					TYPE => "COPY",
					FROM => $module,
					ORIG => $resource,
					PATH => $basename,
					DEST => "$bundle/Contents/Resources/",
				);
			}
			
			$path = "$bundle/Contents/MacOS/$prog";
		}
		
		$link = Compile::Driver::Job::Link::Binary::->new
		(
			TYPE => "LINK",
			FROM => $module,
			OBJS => \@objs,
			PREQ => $module->recursive_prerequisites,
			DEST => $path,
		);
		
		if ( Compile::Driver::Options::installing )
		{
			$copy = Compile::Driver::Job::Copy::->new
			(
				TYPE => "COPY",
				FROM => $module,
				ORIG => $path,
				DEST => "var/out/$prog",
			);
		}
	}
	
	push @jobs, $link  if defined $link;
	push @jobs, $copy  if defined $copy;
	
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

my %running_jobs;

sub spawn
{
	my ( $job ) = @_;
	
	my $pid = fork;
	
	defined $pid or die "fork";
	
	if ( $pid == 0 )
	{
		$job->perform;
		exit 0;
	}
	
	$running_jobs{ $pid } = $job;
}

sub wait_for_one_job
{
	my $pid = wait;
	
	exists $running_jobs{ $pid } or die "No such job $pid\n";
	
	delete $running_jobs{ $pid };
}

sub wait_for_all_jobs
{
	while ( keys %running_jobs )
	{
		wait_for_one_job;
	}
}

sub main
{
	my @args = Compile::Driver::Options::set_options( @_ );
	
	my $configuration = Compile::Driver::Configuration::->new( Compile::Driver::Options::specs );
	
	foreach my $name ( @args )
	{
		my $module = $configuration->get_module( $name, "[mandatory]" );
	}
	
	my $desc = { NAME => "[program args]", DATA => { use => [ @args ] } };
	
	my $module = Compile::Driver::Module::->new( $configuration, $desc );
	
	my @prereqs = @{ $module->recursive_prerequisites };
	
	pop @prereqs;
	
	my @jobs = map { Compile::Driver::jobs_for( $configuration->get_module( $_ ) ) } @prereqs;
	
	my $n_jobs = Compile::Driver::Options::job_count;
	
	if ( $n_jobs > 1 )
	{
		my $last_type = "";
		
		while ( @jobs )
		{
			while ( @jobs  &&  keys %running_jobs < $n_jobs )
			{
				my $job = shift @jobs;
				
				if ( $job->{TYPE} ne $last_type )
				{
					# Don't risk spawning a link while compiles are outstanding
					wait_for_all_jobs;
					
					$last_type = $job->{TYPE};
				}
				
				spawn $job;
			}
			
			# Either we ran out of pending jobs, or maxed out the running jobs
			
			if ( keys %running_jobs )
			{
				wait_for_one_job;
			}
		}
		
		# No more jobs pending
		
		wait_for_all_jobs;
	}
	
	foreach my $job ( @jobs )
	{
		$job->perform;
	}
	
}

1;
