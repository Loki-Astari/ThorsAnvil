AC_INIT([ThorsSerialize], [0.1], [Loki.Astari+ThorsAnvil@gmail.com])
AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([build/autotools/m4])
AC_CONFIG_AUX_DIR([build/autotools/build])

AX_THOR_FUNC_INIT_BUILD([ThorsSerializer], [], [20])

AX_THOR_ENABLE_CONAN

AX_THOR_CHECK_USE_THORS_SERIALIZE([ThorsLogging])
AX_THOR_CHECK_USE_MAGIC_ENUM
AX_THOR_CHECK_USE_YAML
AX_THOR_CHECK_USE_SNAPPY
AX_THOR_CHECK_USE_CRYPTO
AX_THOR_CHECK_USE_ZLIB
AX_THOR_CHECK_USE_EVENT

AX_THOR_DISABLE_TEST_REQUIREING_LOCK_FILES
AX_THOR_DISABLE_TEST_REQUIREING_MONGO_QUERY
AX_THOR_SERVICE_AVAILABLE_MONGO
AX_THOR_CHECK_IS_SLACK_TESTABLE
AX_THOR_CHECK_USE_BOOST([1.70], [], [
    AC_MSG_ERROR([
        Nisse requires boost CoRoutine2
        The minimum version we support is via boost 1.70 please upgrade your boost libraries to this (or later).
   ])
])

LT_INIT

subconfigure="${subconfigure} --with-thorserialize-root=$(pwd)/build --with-nisse-root=$(pwd)/build --disable-slacktest"

AX_THOR_FEATURE_HEADER_ONLY_VARIANT([THORS_SERIALIZER])
AX_THOR_FUNC_TERM_BUILD([SERIALIZER], [localConfigSummary.h:config.h.in])

AC_OUTPUT
