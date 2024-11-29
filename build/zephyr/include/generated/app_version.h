#ifndef _APP_VERSION_H_
#define _APP_VERSION_H_

/*  values come from cmake/version.cmake
 * BUILD_VERSION related  values will be 'git describe',
 * alternatively user defined BUILD_VERSION.
 */

/* #undef ZEPHYR_VERSION_CODE */
/* #undef ZEPHYR_VERSION */

#define APPVERSION                   0x2070000
#define APP_VERSION_NUMBER           0x20700
#define APP_VERSION_MAJOR            2
#define APP_VERSION_MINOR            7
#define APP_PATCHLEVEL               0
#define APP_TWEAK                    0
#define APP_VERSION_STRING           "2.7.0"
#define APP_VERSION_EXTENDED_STRING  "2.7.0+0"
#define APP_VERSION_TWEAK_STRING     "2.7.0+0"

#define APP_BUILD_VERSION de5fee61570b


#endif /* _APP_VERSION_H_ */