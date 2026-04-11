THORSANVIL_ROOT				?= $(realpath ./)
TARGET						= src

include $(THORSANVIL_ROOT)/build/tools/Project.Makefile
include $(THORSANVIL_ROOT)/build/tools/Make/CMakeInstall.Makefile
