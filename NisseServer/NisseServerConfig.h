/* src/NisseServer/NisseServerConfig.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */



#ifndef  THORS_NISSE_CONFIG_H
#define  THORS_NISSE_CONFIG_H
#ifndef  THORS_PACKAGE_INFO_CONFIG_H
#define  THORS_PACKAGE_INFO_CONFIG_H

    

/* define if the Boost library is available */
#define HAVE_BOOST /**/

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* We have found package Event */
/* #undef HAVE_EVENT */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* We have found MagicEnumHeaderOnly package */
#define HAVE_MagicEnumHeaderOnly 1

/* Magic Enum V1 being used */
/* #undef HAVE_MagicEnumHeaderOnlyV1 */

/* Magic Enum V2 being used */
#define HAVE_MagicEnumHeaderOnlyV2 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* We have found package Thors Serializer */
#define HAVE_THORSSERIALIZER 1

/* We have found ThorSerializeHeaderOnly package */
/* #undef HAVE_ThorSerializeHeaderOnly */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Enable to use header only libraries */

/* For header only convert to inline */

/* Check for older build tools and use simpler code with old tools */
#define NO_STD_SUPPORT_FROM_CHAR_DOUBLE 1

/* Check for older build tools and use simpler code with old tools */
/* #undef NO_STD_SUPPORT_TO_CHAR_DOUBLE */

/* Name of package */
#define PACKAGE "nisse"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "Loki.Astari+Nisse@gmail.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Nisse"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Nisse 0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "nisse"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.1"

/* Define to 1 if all of the C89 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.1"




    
#endif


/* Enable to use header only libraries */
#define NISSE_HEADER_ONLY 1

/* For header only convert to inline */
#define NISSE_HEADER_ONLY_INCLUDE inline


#endif
