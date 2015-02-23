Based on Stephan Brumme's [Fast CRC32](http://create.stephan-brumme.com/crc32/)
library.

But with modified tables from Intel's
[Slicing-By-8](http://sourceforge.net/projects/slicing-by-8/), to match
SSE4.2's CRC32 instruction, which uses CRC-32C (also known as Castagnoli CRC32).
