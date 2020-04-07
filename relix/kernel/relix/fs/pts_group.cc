/*
	pts_group.cc
	------------
*/

#include "relix/fs/pts_group.hh"


namespace relix
{

static vfs::dynamic_group pts_group;

vfs::dynamic_group& get_pts_group()
{
	return pts_group;
}

}
