# Source files located under $(CORE_ROOT) are automatically added.
ifeq ($(USER_ROOT),)
  USER_ROOT := ./simulation/
endif

rwildcard    = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
USERCSRC    := $(call wildcard,$(USER_ROOT),*.c)
USERCPPSRC  := $(call rwildcard,$(USER_ROOT),*.cpp)


# Shared variables.
ALLCSRC     += $(USERCSRC)
ALLCPPSRC   += $(USERCPPSRC)

