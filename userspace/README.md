### binary

userspace shit

Usage:

```shell

./binary enable 10000	# filter uid 10000
./binary disable 10000	# do NOT filter uid 10000
./binary reset		# reset bitmap
```


example, to filter all pkg uid:
```shell
for i in $(awk '$2 >= 10000 {print $2}' /data/system/packages.list | sort -n); do /tmp/binary enable $i; done

```
