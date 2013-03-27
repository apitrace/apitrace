This directory contains the source for the generation of wrapper DLLs or
preload shared-objects.

The cmake targets have names different from the true DLLs/shared-objects to
prevent collision when trying to link against the true ones.
