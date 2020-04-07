package Compile::Driver::Module;

use Compile::Driver::Files;
use Compile::Driver::InputFile::SourceList;

use warnings FATAL => 'all';
use strict;


*list_files      = \&Compile::Driver::Files::list;
*enumerate_files = \&Compile::Driver::Files::enumerate;

*read_source_list_file = \&Compile::Driver::InputFile::SourceList::read_file;


sub new
{
	my $class = shift;
	
	my ( $conf, $desc ) = @_;
	
	return bless { CONF => $conf, DESC => $desc }, $class;
}

sub get
{
	my $self = shift;
	
	return $self->{CONF}->get_module( @_ );
}

sub target
{
	my $self = shift;
	
	return $self->{CONF}->target();
}

sub name
{
	my $self = shift;
	
	return $self->{DESC}{NAME};
}

sub product_type
{
	my $self = shift;
	
	return $self->{DESC}{DATA}{ product }[0] || "";
}

sub exports_headers
{
	my $self = shift;
	
	return grep { $self->product_type eq $_ } qw( source lib );
}

sub is_static_lib
{
	my $self = shift;
	
	return grep { $self->product_type eq $_ } qw( lib dropin );
}

sub is_toolkit
{
	my $self = shift;
	
	return $self->product_type eq 'toolkit';
}

sub is_bundle
{
	my $self = shift;
	
	return $self->product_type eq "app"
}

sub is_executable
{
	my $self = shift;
	
	return grep { $self->product_type eq $_ } qw( tool microtool nanotool app );
}

sub is_buildable
{
	my $self = shift;
	
	return $self->is_static_lib  ||  $self->is_toolkit  ||  $self->is_executable;
}

sub program_name
{
	my $self = shift;
	
	return $self->{DESC}{DATA}{ program }[0] || $self->name;
}

sub mac_creator
{
	my $self = shift;
	
	return $self->{DESC}{DATA}{ creator }[0] || "????";
}

sub immediate_prerequisites
{
	my $self = shift;
	
	my $data = $self->{DESC}{DATA};
	
	return map { @{ $data->{ $_ } || [] } } qw( use uses );
}

sub merge_lists
{
	my @lists = @_;
	
	my %seen;
	
	my @result;
	
	foreach my $list ( @lists )
	{
		my @list = grep { !$seen{ $_ } } @$list;
		
		$seen{ $_ }++ foreach @list;
		
		push @result, @list;
	}
	
	return \@result;
}

sub recursive_prerequisites
{
	my $self = shift;
	
	if ( my $memo = $self->{MEMO}{PREREQS} )
	{
		return $memo;
	}
	
	my @immediate = $self->immediate_prerequisites;
	
	my @lists = map { $self->get( $_ )->recursive_prerequisites } @immediate;
	
	push @lists, [ $self->name ];
	
	my $merge = merge_lists( @lists );
	
	return $self->{MEMO}{PREREQS} = $merge;
}

sub imports
{
	my $self = shift;
	
	my $data = $self->{DESC}{DATA};
	
	my $imports = $data->{imports} || [];
	
	my @result = @$imports;
	
	unshift @result, "-L" . $self->tree  if defined $self->{DESC}{PATH};
	
	return @result;
}

sub tree
{
	my $self = shift;
	
	my $desc = $self->{DESC};
	
	return $desc->{ROOT}  if exists $desc->{ROOT};
	
	my $path = $desc->{PATH};
	
	my ( $dir, $conf ) = $path =~ m{^ (.*) /+ ([^/]+) $}x;
	
	$dir =~ s{ /+ A-line\.confd $}{}x;
	
	return $desc->{ROOT} = $dir;
}

sub find_rez
{
	my $self = shift;
	
	my ( $spec ) = @_;
	
	if ( my ( $projname, $rezname ) = $spec =~ /^ (\w+?) : (.+) $/x )
	{
		return $self->get( $projname )->find_rez( $rezname );
	}
	
	return $self->tree . "/Rez/$spec";
}

sub rez_files
{
	my $self = shift;
	
	my $data = $self->{DESC}{DATA};
	
	my $rez = $data->{rez} || [];
	
	return map { $self->find_rez( $_ ) } @$rez;
}

sub resources
{
	my $self = shift;
	
	my $dir = $self->tree . "/Resources";
	
	if ( -d $dir )
	{
		return list_files( $dir, sub {1} );
	}
	
	return;
}

sub info_txt
{
	my $self = shift;
	
	my $info_txt = $self->tree . "/Info.txt";
	
	if ( -f $info_txt )
	{
		return $info_txt;
	}
	
	return;
}

sub source_list
{
	my $self = shift;
	
	my $desc = $self->{DESC};
	
	my $path = $desc->{PATH};
	
	my ( $dir, $conf ) = $path =~ m{^ (.*) /+ ([^/]+) $}x;
	
	my $list = "$dir/Source.list";
	
	return -f $list ? $list : ();
}

sub sources_from_list
{
	my $self = shift;
	
	my ( $list_file ) = @_;
	
	my $dir = $self->tree;
	
	$dir .= "/";  # Insert sentinel
	
	my @sources = read_source_list_file( $list_file );
	
	foreach my $subpath ( @sources )
	{
		my $path = "$dir/$subpath";
		
		die "Can't find $subpath\n"  if !-f $path;
		
		$subpath = $path;
	}
	
	return @sources;
}

sub source_dirs
{
	my $self = shift;
	
	my $tree = $self->tree;
	
	$tree .= "/";  # Insert sentinel
	
	return map { "$tree/$_" } @{ $self->{DESC}{DATA}{sources} || [] };
}

sub search_dirs
{
	my $self = shift;
	
	my $tree = $self->tree;
	
	my @searches = @{ $self->{DESC}{DATA}{search} || [] };
	
	foreach my $search ( @searches )
	{
		next if $search =~ m{^ / }x;  # absolute path
		
		next if $search =~ s{^ ~/ }{$ENV{HOME}/}x;  # home-relative path
		
		$search = $search eq "." ? $tree : "$tree/$search";
	}
	
	if ( !@searches )
	{
		@searches = $tree;
	}
	
	$_ .= "/"  foreach @searches;  # Insert sentinel
	
	return @searches;
}

sub filter
{
	my ( $conf ) = @_;
	
	return  if !/\.(c|cc|cpp)$/;
	
	my @parts = split m{\.}, $_;
	
	pop   @parts;  # filename extension
	shift @parts;  # forename
	
	foreach my $part ( @parts )
	{
		return  if $conf->conflicts_with( $part );
	}
	
	return 1;
}

sub sources
{
	my $self = shift;
	
	return if !$self->is_buildable;
	
	if ( my $list = $self->source_list )
	{
		return $self->sources_from_list( $list );
	}
	
	my @source_dirs = $self->source_dirs;
	
	@source_dirs = $self->search_dirs  if !@source_dirs;
	
	my $conf = $self->{CONF};
	
	my $filter = sub { filter( $conf ) };
	
	return map { enumerate_files( $_, $filter ) } @source_dirs;
}

sub source_is_tool
{
	my $self = shift;
	
	my ( $source ) = @_;
	
	if ( !exists $self->{IS_TOOL} )
	{
		my $tools = $self->{DESC}{DATA}{tools} || [];
		
		$self->{IS_TOOL} = { map { $_ => 1 } @$tools };
	}
	
	$source =~ s{^ .* / }{}x;
	
	return exists $self->{IS_TOOL}{ $source };
}

sub all_search_dirs
{
	my $self = shift;
	
	return $self->{MEMO}{DIRS}  if exists $self->{MEMO}{DIRS};
	
	my @prereqs = @{ $self->recursive_prerequisites };
	
	pop @prereqs;  # Remove this project
	
	@prereqs = reverse @prereqs;
	
	@prereqs = grep { $_->exports_headers } map { $self->get( $_ ) } @prereqs;
	
	my @search_dirs;
	
	@search_dirs = $self->search_dirs;
	
	my %seen = map { $_ => 1 } @search_dirs;
	
	foreach my $prereq ( @prereqs )
	{
		my @dirs = grep { !$seen{ $_ } } $prereq->search_dirs;
		
		%seen = (%seen, map { $_ => 1 } @dirs);
		
		push @search_dirs, @dirs;
	}
	
	return $self->{MEMO}{DIRS} = \@search_dirs;
}

sub find_include_in_search_dirs
{
	my ( $subpath, $search_dirs ) = @_;
	
	foreach my $dir ( @$search_dirs )
	{
		my $path = "$dir/$subpath";
		
		return $path  if -f $path;
	}
	
	return;
}

my %include_cache;

sub find_include
{
	my $self = shift;
	
	my ( $subpath ) = @_;
	
	my $key = "$self $subpath";
	
	return $include_cache{ $key }  if exists $include_cache{ $key };
	
	return $include_cache{ $key } = find_include_in_search_dirs( $subpath, $self->all_search_dirs );
}

1;
