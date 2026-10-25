# NOTE: unity build. single unit.

obj-$(CONFIG_FGETPRIORITY) += fgetpriority.o
fgetpriority-y := main.o

CFLAGS_main.o +=  $(call cc-option, -std=gnu11)
CFLAGS_main.o +=  $(call cc-option, -std=gnu17)
CFLAGS_main.o +=  $(call cc-option, -std=gnu23)
CFLAGS_main.o +=  $(call cc-option, -std=gnu2y)

CFLAGS_main.o += $(call cc-option, -Wno-declaration-after-statement)
CFLAGS_main.o += $(call cc-option, -Wno-deprecated-octal-literals)

ifeq ($(CONFIG_CC_OPTIMIZE_FOR_SIZE),y)
CFLAGS_REMOVE_main.o += -Oz
CFLAGS_REMOVE_main.o += -Os
CFLAGS_main.o += -O2
endif

CFLAGS_REMOVE_main.o += -fno-builtin

CFLAGS_main.o += -g0 -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer
CFLAGS_main.o += $(call cc-option, -momit-leaf-frame-pointer)

CFLAGS_REMOVE_main.o += $(CC_FLAGS_FTRACE)
CFLAGS_REMOVE_main.o += -pg

CFLAGS_REMOVE_main.o += -ftrivial-auto-var-init=pattern
CFLAGS_REMOVE_main.o += -ftrivial-auto-var-init=zero
CFLAGS_REMOVE_main.o += -enable-trivial-auto-var-init-zero-knowing-it-will-be-removed-from-clang

# if cflags can be macro'd, this will be called 'TRUST_ME'
CFLAGS_main.o += -fno-stack-protector -fno-stack-check
CFLAGS_REMOVE_main.o += -fsanitize=shadow-call-stack

