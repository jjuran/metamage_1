Damogran
========

Damogran is a lossless, byte-oriented RLE compression scheme.  It's designed for compressing screencast video frame deltas at depths between 1 and 8 bits.

Damogran has several improvements over PackBits:

  * a subsequence of 256 repeating bytes packs to two bytes, not six
  * longer repeating subsequences of up to 65536 bytes are packed O(1) in space (four bytes), not O(n) (up to 1034 bytes)
  * each packed subsequence (compressed or not) has two-byte alignment
  * NUL bytes form a no-op subsequence, allowing packed data to be padded to a larger alignment as needed

Damogran excels when there are a few islands of interesting data amid large expanses of repeating bytes (whether zero or something else).  It's not suitable for compressing individual complex images (use PNG).  Text is right out (use gzip).

Damogran requires that the unpacked source be an even number of bytes and expects it to begin on an even byte boundary, but this is considered a feature rather than a bug.

Specification
-------------

Source data must be an even number of bytes.  Valid packed data is also an even number of bytes.

Zero-length source produces zero-length packed data and vice versa.  (Implementations may decline to provide zero-length conversions.  Users of such implementations are responsible for handling (or rejecting) this case on their own.)

Packed data is examined two bytes at a time, which we'll call `c0` and `c1` and interpret as twos' complement signed integers (i.e. int8_t).

Negative values of `c0` are reserved.

Positive values of `c0` indicate `n = 2 * (c0 + 1)` occurrences of the byte `c1` (where `n` is an even number between 4 and 256 inclusive).

If `c0` is zero, then consider `c1`:

Negative values of `c1` indicate (and consume) the `n = 2 * -c1` bytes immediately following (where `n` is an even number between 2 and 256 inclusive).

Positive values of `c1` consume the next two bytes as `c2` and `c3`, and indicate `n = 2 * (c1 * 256 + c2 + 1)` occurrences of `c3`, with `c2` regarded as unsigned (where `n` is an even number between 514 and 65536 inclusive).

If `c1` is zero, the subsequence has no effect on the unpacked source.

Users of untrusted packed data (such as video players) are responsible for validating the data's internal consistency prior to unpacking it.
