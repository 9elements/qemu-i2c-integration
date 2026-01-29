# qemu-i2c-integration

Integration project for the work on the QEMU i2c chardev device. Motivated by work
for OpenPRoT.

## Overview

In the scope of the OpenPRoT project we needed a QEMU with an I2C device that can
support an external application and also allow MCTP over I2C communication.

While the implementation in QEMU should be generic enough. The integration focuses
on testing/developing this with MCTP via I2C in mind.

The `task run-qemu` command will start a qemu that will loop a i2c bus into itself
via a chardev. The internal I2C bus 1 will be connected to the bus 2. With two
different devices.
This should allow an echo communication.

## Submodules

This project uses the following submodules:

- **QEMU**: `git@github.com:9elements/qemu.git` (branch: `groenke/wip/i2c_chardev`)
- **Buildroot**: `git@github.com:mynetz/buildroot.git` (branch: `feat/ast2600-nvme-mi`)

To initialize the submodules after checkout:
```bash
task init-submodules
```

## Contents

This repository contains these components:
- QEMU i2c chardev device implementation (via `qemu` submodule)
- A buildroot environment to run inside QEMU (target) for tests and development
  (via `buildroot` submodule)
- Tests and programs for the i2c chardev device for the host and target side
    - See `br2-external` for the buildroot external tree with custom packages
    - No host programs for now
- Documentation and examples for using the i2c chardev device

## Building

The project uses [Task](https://taskfile.dev/) for build automation.

### Quick Start

```bash
# 1. Initialize submodules
task init-submodules

# 2. Build QEMU
task build-qemu

# 3. Build Buildroot
task build-buildroot

# 4. Run QEMU
task run-qemu
```

Note: Building QEMU and Buildroot can take a while. 10-20 minutes on a fast machine. There
is an option to provide a pre-build toolchain for Buildroot. This is currently missing in
this PoC. The already prebuild toolchain from Bootlin lacks the kernel headers.
This is a problem for the MCTP tooling. Building all is a workaround for now.

## BR2_EXTERNAL Structure

The `br2-external` directory contains a Buildroot external tree with custom packages and
configurations:

### Packages

- **mctp-echo**: A simple test application that currently prints a hello world message. Will be
- extended to support MCTP over I2C testing.

### Notes

- The dependency on the custom `buildroot` can probably be droppen, if the custom changes
  are moved to the external tree in this repository. Right now we re-use some previuosly
  done work for another project.
- The buildroot project does not check the hashes for the downloaded archives right now.
  something in the u-boot build triggered a wrong hash check and broke the build.
- The available mctp devices should look like this
  ```shell
  mctp l
  dev lo index 1 address 00:00:00:00:00:00 net 1 mtu 65536 up
  dev mctpi2c1 index 6 address 0x10 net 1 mtu 254 down
  dev mctpi2c2 index 7 address 0x20 net 1 mtu 254 down
  ```
