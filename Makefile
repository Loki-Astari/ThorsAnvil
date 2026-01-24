THORSANVIL_ROOT				?= $(realpath ./)
TARGET						= src

include $(THORSANVIL_ROOT)/build/tools/Project.Makefile

all:					apps_$(PLATFORM)_all
install: 				apps_$(PLATFORM)_install

.PHONY:		apps_Linux_all apps_Darwin_all apps_MSYS_NT_all apps_MINGW64_NT_all
apps_Linux_all:				apps_all
apps_Darwin_all:			apps_all
apps_MSYS_NT_all:
apps_MINGW64_NT_all:

.PHONY:	apps_Linux_install apps_Darwin_install apps_MSYS_NT_install apps_MINGW64_NT_install
apps_Linux_install:			apps_install
apps_Darwin_install:		apps_install
apps_MSYS_NT_install:
apps_MINGW64_NT_install:

.PHONY:		apps_all apps_install
apps_all:
	$(MAKE) -C apps all
apps_install:
	$(MAKE) -C apps install



