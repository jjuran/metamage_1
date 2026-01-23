ams-core:  Advanced Mac Substitute System Core
==============================================

Cursor Management
-----------------

  * 16x16 black and white cursor
    * supports emulated hardware cursor
    * supports multibit displays up to 8-bit depth in software
      * TODO: 16-bit and 32-bit depth
  * TODO:  uncoupled cursor

Gestalt Manager
---------------

  * patches _Gestalt
    * TODO: 'tabl' selector
  * TODO: _NewGestalt, etc.

Memory Manager
--------------

  * internal-use pseudo-heapzone that allocates in unused ROM / I/O space
    * persistent across application launches
    * doesn't occupy limited RAM space (or even more limited SysHeap)
    * used for e.g. statically allocated reusable temp regions
  * system and application heapzones for non-relocatable (Ptr) blocks
    * TODO: resize blocks in _SetPtrSize
  * relocatable (Handle) blocks using non-standard wide master pointers
    * TODO: migrate to a true heapzone-based implementation
  * handle state accessors

Handle Utilities
----------------

  * _Munger
  * _HandToHand, _PtrToHand, _PtrToXHand, _HandAndHand, _PtrAndHand

Reactor-Pattern Event Source
----------------------------

  * idles until a timer expires or an fd becomes readable

OS Event Manager
----------------

  * _GetOSEvent throttles _SystemTask if _GetNextEvent isn't called
  * TODO: _PPostEvent

Event Manager
-------------

  * _WaitNextEvent integrated with the event reactor
  * _GetNextEvent is throttled by default to save CPU/battery
  * TODO: FKEYs
  * TODO: GNE filter
  * TODO: alarm clock?
  * TODO: move to ams-ui

Vertical Retrace Manager
------------------------

  * VBL queue integrated with the event reactor
  * works around SoundMusicSys bugs in Lemmings and Prince of Persia

Patch Manager
-------------

  * _GetTrapAddress, _SetTrapAddress

Operating System Utilities
--------------------------

  * _Enqueue. _Dequeue
  * _Microseconds
  * _Delay is integrated with the event reactor for efficient sleep
  * _CmpString, _RelString, _UprString (TODO: implement MARKS bit)
  * _ReadDateTime (just returns Time)
  * _Date2Secs, _Secs2Date
  * _SysBeep shim writes $07 to the console (patched by ams-io)
  * _SysEnvirons hard-coded for Mac Plus

Desk Manager
------------

  * _SystemTask (TODO: give time to drivers)
  * _SystemEdit and _SystemEvent shims always return false (no DA support yet)

System Error Manager
--------------------

  * installs exception handlers in the system vector table that call _SysError
  * _SysError shim logs to the console and exits the emulation
    * (patched by ams-dsat and/or vdb)

Debugging
---------

  * _Debugger and _DebugStr shims write to the console (patched by vdb)

Parameter RAM
-------------

  * _ReadXPRam shim unconditionally zeroes the caller's buffer
  * _InitUtil and _WriteParam are no-ops
