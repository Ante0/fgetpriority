### fgetpriority

A small LKM to filter sys_getpriority

Usage (if LKM):

```shell
#!/bin/sh

kptr_set=$(cat /proc/sys/kernel/kptr_restrict)
echo 1 > /proc/sys/kernel/kptr_restrict
ptr_address=$(grep " sys_call_table$" /proc/kallsyms | awk {'print "0x"$1'})
insmod fgetpriority.ko sct_param="$ptr_address"
echo $kptr_set > /proc/sys/kernel/kptr_restrict
```

Compatibility:
- CONFIG_KALLSYMS_ALL=y is required if built as module.

