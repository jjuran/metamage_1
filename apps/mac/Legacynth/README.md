Legacynth
=========

[Legacynth][] implements the [Sound Driver API][SoundDriver] in traditional Mac OS environments that don't.

[Legacynth]:  <https://www.macrelics.com/legacynth/>

[SoundDriver]:  <../../libs/SoundDriver/README.md>

Initially, the Sound Driver was the only high-level API for playing sound.  Subsequently, the Sound Manager API was added to make better use of the Apple Sound Chip in the [Mac II][].  [System 6][] switched to using Sound Manager for `SysBeep()` on all machines, with the Sound Driver API retained for backward compatibility.

[Mac II]:    <https://en.wikipedia.org/wiki/Macintosh_II>
[System 6]:  <https://en.wikipedia.org/wiki/System_6>

Support for the Sound Driver API was dropped in AV Mac models and all [PowerPC-based Macs][PowerMac].  A `.Sound` driver remains, but it doesn't play sound.

[PowerMac]:  <https://en.wikipedia.org/wiki/Power_Macintosh>

Legacynth restores Sound Driver API support by intercepting `Write` and `KillIO` calls to the `.Sound` driver, and generating sounds using the Sound Manager.

**Legacynth Demo** implements the Sound Driver's **free-form** synthesizer.

To use it:

  * Place the **Legacynth Extension** in the Extensions folder and restart.
  * Open **Legacynth Demo** and click Start.  (If the button is labeled "Quit", then you can't use Legacynth on this system.)
  * Open **Sound Check** and click Test to verify that it works.

Feel free to contact me by email at `jjuran@gmail.com` or in `#mac68k` on Libera.Chat with any questions or other comments.

-- Josh Juran
