ifeq (${_ARCH_},armeb)
PLATFORM_CPPFLAGS += -mbig-endian
LDFLAGS_FINAL += -EB -be8
endif
