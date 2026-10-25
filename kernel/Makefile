obj-$(CONFIG_FGETPRIORITY) += fgetpriority.o
fgetpriority-y := main.o

ccflags-y +=  $(call cc-option, -std=gnu11)
ccflags-y +=  $(call cc-option, -std=gnu17)
ccflags-y +=  $(call cc-option, -std=gnu23)
ccflags-y +=  $(call cc-option, -std=gnu2y)

ccflags-y += -g0
ccflags-y += $(call cc-option, -Wno-declaration-after-statement)
ccflags-y += $(call cc-option, -Wno-deprecated-octal-literals)

ccflags-y += -fno-stack-protector
