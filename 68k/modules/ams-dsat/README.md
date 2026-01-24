ams-dsat:  Advanced Mac Substitute Diagnostics, Simple And Terse
================================================================

Depends on:

  * [ams-qd][]

System Error Manager
--------------------

  * patches _SysError, displays conventional System Error pseudo-dialog
    * calls _WaitNextEvent (no busy-looping)
      * TODO: when WNE moves to ams-ui, that becomes a prerequisite
    * TODO: reword "Restart", which is inaccurate

[ams-qd]:  <../ams-qd/#readme>
