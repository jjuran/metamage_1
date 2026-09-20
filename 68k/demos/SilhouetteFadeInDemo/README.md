Silhouette fade-in demo
=======================

This application does a lo-fi fade-in of a region.

The region format is the in-memory format of a `RgnHandle` in classic Mac OS.

The sample region is derived from [work][src] by [Tkgd2007][] and is licensed
under the Creative Commons [Attribution-ShareAlike 3.0 Unported][by-sa] license.

The [330px PNG image][330] was converted to a bi-level bitmap using `img2bits`
(lossily discarding anti-aliasing).
The resulting BITS image was then losslessly scanned into a region with `bits2rgn`.

[Tkgd2007]:  <https://commons.wikimedia.org/wiki/User:Tkgd2007>

[by-sa]:  <https://creativecommons.org/licenses/by-sa/3.0/deed.en>

[src]:  <https://en.wikipedia.org/wiki/File:Human_evolution.svg>
[330]:  <https://thumb.wikimedia.org/wikipedia/commons/thumb/6/69/Human_evolution.svg/330px-Human_evolution.svg.png>
