ams-io:  Advanced Mac Substitute I/O Subsystem
==============================================

Depends on:

  * [ams-core][]

Device Manager
--------------

  * _Open, _Close, _Read, _Write (patched by ams-fs)
  * _Control, _Status, _KillIO
  * sync, async, and immed I/O requests
    * sync calls integrated with the event reactor for efficient sleep
  * _DrvrInstall
  * works around SoundMusicSys stack smashing bug in Prince of Persia
  * reference-counts .Sound I/O requests using the same parameter block
    * works around SoundMusicSys I/O queue corruption bug in Prince of Persia

Serial Driver
-------------

  * Console Port (".CIn" and ".COut") on fds 0 and 1
  * Modem Port (".AIn" and ".AOut") on configurable fd (optional)

Sound Driver
------------

  * patches _SysBeep to play a square-wave tone (patched by ams-ui)
    * TODO: implement an authentic Simple Beep sound
  * forwards requests to [sndtrack][] for native processing, 16-bit output
    * square-wave mode
    * free-form mode
    * four-tone mode
      * VBL task observes FTSoundRec changes and updates the external synth
        * including changes to the wave tables themselves

[ams-core]:  <../ams-core/#readme>
[sndtrack]:  <../../../sound/sndtrack/#readme>
