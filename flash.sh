#!/bin/bash

[ -z "$1" ] && {
	echo "need device as argument" >&2
	exit 1
}

topdir="$(realpath "$(dirname "$0")")"
device="$1"

mounted_partitions() {
	mount | grep "^${device}" | cut -d' ' -f1
}

for part in $(mounted_partitions); do
	umount "$part"
done

dd if="${topdir}/buildroot/output/images/sdcard.img" of="$device" bs=1M
