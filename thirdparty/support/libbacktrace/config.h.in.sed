s:^/\* config\.h.\in\.  Generated from configure\.ac by autoheader\.  \*/$:/* config.h.in.  Generated from libbacktrace/config.h.in by apitrace/thirdparty/support/libbacktrace/config.h.in.sed.  */:

s/^#undef \(HAVE_\S\+\)$/#cmakedefine \1 1/
s/^#undef \(\S\+\)$/#cmakedefine \1 @\1@/
