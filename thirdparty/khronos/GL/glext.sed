# MacOSX doesn't follow the official glext.h definition for GLhandleARB, and
# instead defines it as:
#
#   typedef void *GLhandleARB;
#
# But to avoid integer to pointer conversion issues, we simply use 'unsigned
# long' which should be the same size on all supported architectures.

/^typedef unsigned int GLhandleARB;$/ {

    i\
#ifdef __APPLE__\
typedef unsigned long GLhandleARB;\
#else

    a\
#endif

}
