# artifact of qtcreator which happens to be currently my primary IDE
# as a consequence of a stupid frustrating lack of feature, unless you
# (incorrectly) add a header file to a cmake target, it won't show up in
# 'Projects' view navigation window. So making a pseudo target here and
# adding the relevant header files

set(lib_HEADERS SOURCES
    utils/random.h utils/random.ipp
    )

add_custom_target(lib_HEADERS ${lib_HEADERS})
