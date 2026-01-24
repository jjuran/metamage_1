ams-drvr:  Advanced Mac Substitute Driver Loader
================================================

Depends on:

  * [ams-rsrc][]

Driver Loader
-------------

  * patches _Open
    * if opening a driver fails, tries to load it from a resource
      * if successful, installs it in the unit table, then opens it

[ams-rsrc]:  <../ams-rsrc/#readme>
