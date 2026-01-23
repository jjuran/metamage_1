ams-rsrc:  Advanced Mac Substitute Resource Manager
===================================================

Depends on:

  * [ams-fs][]

Resource Manager
----------------

  * _InitResources runs 'INIT' resources
  * add/change/remove resources
  * resfile compaction
  * _OpenRFPerm
  * _Count1Types, _Get1IxType, etc.
  * TODO: _CountTypes, _GetIndType (requires filtering duplicates)
  * TODO: _UniqueID
  * TODO: implement _LoadResource, honor ResLoad=false
  * TODO: implement _SetResPurge (but purging doesn't happen yet)
  * detects and reports compressed resources (TODO: expand them (WIP))
  * TODO: return on-disk size in _SizeRsrc (WIP)

[ams-fs]:  <../ams-fs/#readme>
