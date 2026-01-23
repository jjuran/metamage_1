ams-fs:  Advanced Mac Substitute File System
============================================

Depends on:

  * [ams-io][]

Filesystem Support
------------------

  * disk images
    * read-only MFS
    * read-only MacBinary / MacBinary+ (non-nested only)
    * TODO: DiskCopy 4.2 images
    * TODO: HFS volumes
  * shared directories via [Freemount][] protocol
    * read-only Bootstrap volume (typically ../ams-68k-bin/mnt/)
      * used to load disk images
      * scanned by 'INIT' 31 for INIT-bearing files (Mac hosts only)
      * can provide unencoded applications to launch (Mac hosts only)
    * read/write Documents volume (e.g. ~/Documents/AMS/AppName)
    * read/write Preferences volume (e.g. ~/Library/Preferences/AMS/AppName)
    * writeable resource forks and metadata for docs and prefs (Mac hosts only)
    * non-destructive persistence for writes to the application data fork
      * (e.g. ~/Library/Preferences/AMS/AppName/data (TODO: rename))
    * TODO: use AppleDouble for rsrc/metadata on non-Macs
    * TODO: consider non-MacRoman Unicode filename idioms as signals
      * e.g. to make archive encoding transparent to the emulation
    * TODO: reimplement Freemount client to be non-blocking
    * TODO: choose a zero-width character to represent NUL in filenames

File Manager
------------

  * patches _Open, _Close, _Read, _Write
  * _GetEOF, _SetEOF, _GetFPos, _SetFPos
  * _GetFileInfo, _SetFileInfo
  * _FlushFile
  * _Create
  * _Delete shim returns errors when expected, but doesn't delete (TODO)
  * TODO: HFS APIs

[ams-io]:  <../ams-io/#readme>

[Freemount]:  <https://www.freemount.org/>
