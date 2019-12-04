/*
	con_group.cc
	------------
*/

#include "relix/fs/con_group.hh"


namespace relix
{

static vfs::dynamic_group con_group;

vfs::dynamic_group& get_con_group()
{
	return con_group;
}

}
