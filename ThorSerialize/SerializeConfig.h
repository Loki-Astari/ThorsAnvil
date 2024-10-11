/* src/Serialize/SerializeConfig.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */



#ifndef  THORS_SERIALIZER_CONFIG_H
#define  THORS_SERIALIZER_CONFIG_H
#ifndef  THORS_PACKAGE_INFO_CONFIG_H
#define  THORS_PACKAGE_INFO_CONFIG_H

    

/* We have found package CRYPTO */
/* #undef HAVE_CRYPTO */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* We have found MagicEnumHeaderOnly package */
/* #undef HAVE_MagicEnumHeaderOnly */

/* We have found package SNAPPY */
/* #undef HAVE_SNAPPY */

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

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* We have found package YAML */
/* #undef HAVE_YAML */

/* We have found package ZLIB */
/* #undef HAVE_ZLIB */

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* "Get mongosh version into #define. That way we can turn off some tests" */
#define MONGO_MAJOR_VERSION 7

/* Check for older build tools and use simpler code with old tools */
#define NO_STD_SUPPORT_FROM_CHAR_DOUBLE 1

/* Check for older build tools and use simpler code with old tools */
/* #undef NO_STD_SUPPORT_TO_CHAR_DOUBLE */

/* Name of package */
#define PACKAGE "thorsserialize"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "Loki.Astari+ThorsAnvil@gmail.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "ThorsSerialize"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ThorsSerialize 0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "thorsserialize"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.1"

/* Define to 1 if all of the C89 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Enable to use header only libraries */

/* For header only convert to inline */

/* Set up to build on Conan */
#define THOR_CONAN 0

/* Disable test that require files to be locked */
#define THOR_DISABLE_TEST_WITH_LOCKED_FILES 0

/* Disable test that require the Mongo server to support the OP_QUERY command
   */
#define THOR_DISABLE_TEST_WITH_MONGO_QUERY 0

/* Name of package for header files */
#define THOR_PACKAGE_NAME ThorsSerializer

/* mongosh DB for testing */
#define THOR_TESTING_MONGO_DB "test"

/* mongosh DB host for testing */
#define THOR_TESTING_MONGO_HOST "127.0.0.1"

/* mongosh DB password for testing */
#define THOR_TESTING_MONGO_PASS "testPassword"

/* mongosh DB user for testing */
#define THOR_TESTING_MONGO_USER "test"

/* Version number of package */
#define VERSION "0.1"




    
#endif


/* Enable to use header only libraries */
#define THORS_SERIALIZER_HEADER_ONLY 1

/* For header only convert to inline */
#define THORS_SERIALIZER_HEADER_ONLY_INCLUDE inline


#endif
