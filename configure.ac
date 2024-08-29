AC_INIT([ThorsSerialize], [0.1], [Loki.Astari+ThorsAnvil@gmail.com])
AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([build/autotools/m4])
AC_CONFIG_AUX_DIR([build/autotools/build])

AX_THOR_FUNC_INIT_BUILD([ThorsSerializer], [], [20])

AX_THOR_ENABLE_CONAN

AX_THOR_CHECK_USE_MAGIC_ENUM
AX_THOR_CHECK_USE_YAML
AX_THOR_CHECK_USE_SNAPPY
AX_THOR_CHECK_USE_CRYPTO
AX_THOR_CHECK_USE_ZLIB

AX_THOR_DISABLE_TEST_REQUIREING_LOCK_FILES
AX_THOR_DISABLE_TEST_REQUIREING_MONGO_QUERY
AX_THOR_SERVICE_AVAILABLE_MONGO

AX_THOR_FUNC_TEST_COMP([
#include <utility>
int main() {
    std::index_sequence<1,2,3,4>    seq;
}])

AX_THOR_FUNC_TEST_COMP_ACTION([[
#include <charconv>
int main() {
    char    buffer[100];
    double  value = 12.345;
    auto result = std::to_chars(buffer, buffer+100, value, std::chars_format::fixed, 6);
}]],
    [
        AC_MSG_NOTICE([Supports std::to_chars(beigin, end, 12.4])
    ],
    [
        AC_MSG_NOTICE([Setting HOMEBREW_OLD_VERSION_OF_MAC])
        AC_DEFINE([HOMEBREW_OLD_VERSION_OF_MAC], [1], [Check for older build tools and use simpler code with old tools])
        subconfigure="${subconfigure} --with-homebrewCLT=${with_homebrewCLT}"
    ]
)

LT_INIT

subconfigure="${subconfigure} --with-thorserialize-root=$(pwd)/build "

AX_THOR_FEATURE_HEADER_ONLY_VARIANT([THORS_SERIALIZER])
AX_THOR_FUNC_TERM_BUILD([SERIALIZER], [src/Serialize/SerializeConfig.h:config.h.in])

AC_OUTPUT
