ams-pack:  Advanced Mac Substitute Package Inventory
====================================================

Depends on:

  * [ams-rsrc][]

Package Manager
---------------

  * _InitAllPacks calls GetResource( 'PACK', id ) directly
  * _InitPack is a no-op (launcher calls InitAllPacks())
  * _Pack0 .. _Pack7
    * handler checks in AppPacks, then GetResource(), then SysError()
      * _Pack2 no-ops DILoad first
      * TODO: InitAllPacks() call makes resource load redundant, axe it
    * _Pack0 patched by ams-ui (List Manager)
    * _Pack3 patched by ams-ui (Standard File)
    * _Pack6 patched by ams-ui (International Utilities)
    * _Pack7 patched by ams-ui (Binary-Decimal Conversion)

Disk Initialization Package
---------------------------

  * DILoad shim is a no-op

Floating Point
--------------

  * Mac Plus ROM's 'PACK' 4 and 5 resources just work, if present

[ams-rsrc]:  <../ams-rsrc/#readme>
