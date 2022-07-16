# Exokernel

Related to a EPITA System and Security Laboratory conference. This is a research project on opensource exokernels.
Contact: `daniel.frederic@lse.epita.fr` or `daniel.frederic@epita.fr`
Related conference link coming soon (in french)

## Current status

Build status and execution on various gcc and clang:

[![Raccon-exok](https://github.com/DaemonOnUnix/Raccoon-exok/actions/workflows/ci.yml/badge.svg)](https://github.com/DaemonOnUnix/Raccoon-exok/actions/workflows/ci.yml)

## Basic build / test kernel init

```
# Generate iso_root
make
# Once you created your elf, edited the limine submodules in iso_root/limine.cfg and put your elf file. defaults hangs all cores.
# This generate the iso $(KERNEL).iso
make iso
# To test in qemu for x86_64
make test
```
