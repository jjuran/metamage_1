vdb:  v68k Debugger
===================

v68k Debugger
-------------

  * patches _Debugger, _DebugStr
  * patches _SysError
  * replaces unimplemented trap handler
  * installs exception handlers in system vector table
    * installs SIGINT interrupt handler (terminal Ctrl-C invokes debugger)
