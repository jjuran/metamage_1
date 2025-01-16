Pearl
=====

This program acts as a front end to [Advanced Mac Substitute][AMS], built upon the [SDL2][SDL] library, which supports targeting a [number of platforms][Installation].  Intended as a drop-in replacement for [Amber][]/[Amethyst][]/[Amphitheatre][], it integrates [libsndtrack][] directly, providing support for the [sndpipe][] protocol in addition to handling 1-bit SKIF files (optionally including cursors) and events as expected.  Much of the shared logic common to all front ends has been moved to the [frontend-common][] library.

[AMS]:  <https://www.v68k.org/advanced-mac-substitute/>

[SDL]:           <https://www.libsdl.org/>
[Installation]:  <https://wiki.libsdl.org/SDL2/Installation>

[Amber]:         <../../mac/Amber/>
[Amethyst]:      <../../mac/Amethyst/>
[Amphitheatre]:  <../../mac/Amphitheatre/>

[libsndtrack]:  <../../../sound/libsndtrack/>
[sndpipe]:      <../../../sound/sndpipe-api/>

[frontend-common]:  <../../../graphics/frontend-common/>

Special thanks to Josh Juran for developing [amicus][], on which Pearl is originally based.

[amicus]:  <../../../mac/libs/amicus/>

Pearl is copyright 2024-2025 Keith Kaisershot.  All rights reserved (unless explicitly stated otherwise).  Pearl is free software; you may use it under the terms of the [GNU AGPL version 3][AGPL] (or later).

[AGPL]:  <../../../LICENSE/AGPL-3.0.txt>
