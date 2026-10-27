#!/system/bin/sh

MODDIR=${0%/*}
KO="$MODDIR/ko/fgetpriority.ko"

kptr_set=$(cat /proc/sys/kernel/kptr_restrict)

echo 1 > /proc/sys/kernel/kptr_restrict

native_sct=$(grep " sys_call_table$" /proc/kallsyms | awk {'print "0x"$1'})
compat_sct=$(grep " compat_sys_call_table$" /proc/kallsyms | awk {'print "0x"$1'})

insmod $KO native_sct_param="$native_sct" compat_sct_param="$compat_sct"

echo $kptr_set > /proc/sys/kernel/kptr_restrict
