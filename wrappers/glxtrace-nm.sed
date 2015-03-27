# Filter-out 'T' symbols
/^[0-9a-fA-F]\+ T /!d
s/^[0-9a-fA-F]\+ T //

# Filter-out expected symbols
/^_init$/d
/^_fini/d
/^dlopen$/d
/^gl[A-Z][0-9A-Za-z_]\+$/d

# Warning
s/^/warning: unexpected glxtrace.so symbol /p
