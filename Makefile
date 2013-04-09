
THORSANVIL_ROOT           = $(realpath ./)

CXXFLAGS                 += -DCLOUD_USE_CPLUSPLUS11 -Wall -Wextra -Wstrict-aliasing -pedantic -Werror -Wunreachable-code -Wno-long-long
TARGET				 = Json Serialize

include $(THORSANVIL_ROOT)/build/tools/Project.Makefile




