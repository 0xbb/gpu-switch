#!/usr/bin/env bash
# Copyright (c) 2014-2015 Bruno Bierbaumer, Andreas Heider

readonly sysfs_efi_vars='/sys/firmware/efi/efivars'
readonly efi_gpu='gpu-power-prefs-fa4ce28d-b62f-4c99-9cc3-6815686e30f9'

usage(){
  cat <<EOF
Usage:
  $(basename $0) --integrated   # Switch to the integrated GPU
  $(basename $0) --dedicated    # Switch to the dedicated GPU   
  $(basename $0) --help         # Show this message

Switches between the integrated and dedicated graphics cards of a dual-GPU
MacBook Pro for the next reboot.

Arguments:
  -i, --integrated
  -d, --dedicated
  -h, --help

Tested hardware:
  MacBook Pro 5,2  (Early 2009, Non-Retina)
  MacBook Pro 5,3  (Mid   2009, Non-Retina)
  MacBook Pro 8,2  (Late  2011, Non-Retina)
  MacBook Pro 9,1  (Mid   2012, Non-Retina)
  MacBook Pro 10,1 (Mid   2012, Retina)
  MacBook Pro 11,3 (Late  2013, Retina)
  MacBook Pro 11,5 (Mid   2015, Retina)
EOF
}

switch_gpu(){
  if ! [ -d /sys/firmware/efi ]; then
    printf "Fatal: $(basename $0) has to be run in EFI mode.\n" 1>&2
    exit 1
  fi
  
  if ! mount | grep -q $sysfs_efi_vars; then
    if ! mount -t efivarfs none $sysfs_efi_vars; then
      printf "Fatal: Couldn't mount '${sysfs_efi_vars}'.\n" 1>&2
      exit 1
    fi
  fi
  chattr -i "${sysfs_efi_vars}/${efi_gpu}" 2> /dev/null
  printf "\x07\x00\x00\x00\x${1}\x00\x00\x00" > "${sysfs_efi_vars}/${efi_gpu}" 
}

if [ $# -ne 1 ]; then
  usage 1>&2
  exit 1
fi

case "$1" in
  -i|--integrated)
    switch_gpu 1
  ;;
  -d|--dedicated)
    switch_gpu 0
  ;;
  -h|--help)
    usage 
  ;;
  *)
    usage 1>&2
    exit 1
  ;;
esac
