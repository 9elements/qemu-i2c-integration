# qemu-i2c-integration

Integration project for the work on the QEMU i2c chardev device. Motivated by work
for OpenPRoT.

## Overview

In the scope of the OpenPRoT project we needed a QEMU with an I2C device that can
support an external application and also allow MCTP over I2C communication.

While the implementation in QEMU should be generic enough. The integration focuses
on testing/developing this with MCTP via I2C in mind.

## Usage

The project uses [Task](https://taskfile.dev/) for build automation. Available tasks can be viewed with `task --list`.

### Running a Test Cycle

To test the MCTP over I2C communication:

1. **Start the echo server** (host side):
   ```bash
   task test-echo
   ```
   This starts the host MCTP I2C echo server that listens on a Unix socket (`vi2c_bus.sock`).

2. **Run QEMU** (in another terminal):
   ```bash
   task run-qemu
   ```
   This starts QEMU with the AST2600 EVB configuration and connects to the echo server via the chardev.

3. **Connect to the serial console** (in another terminal):
   ```bash
   task run-serial
   ```
   This connects to the QEMU serial console.

4. **Run the MCTP echo test** inside QEMU:
   - Login as `root` (no password required)
   - Run the MCTP echo application:
     ```bash
     mctp-echo
     ```

### Other Useful Tasks

- **Connect to QEMU monitor**:
  ```bash
  task run-monitor
  ```

- **Run QEMU with debug tracing** (chardev_i2c traces):
  ```bash
  task run-qemu-debug
  ```

- **Rebuild custom packages** (mctp-echo, mctp-init):
  ```bash
  task rebuild-packages
  ```

## Submodules

This project uses the following submodules:

- **QEMU**: `git@github.com:9elements/qemu.git` (branch: `groenke/wip/i2c_chardev`)
- **Buildroot**: `git@github.com:mynetz/buildroot.git` (branch: `feat/ast2600-nvme-mi`)
- **Host MCTP I2C Tools**: Rust-based tools for testing MCTP over I2C (see `host-mctp-i2c-tools/`)

## Contents

This repository contains these components:
- QEMU i2c chardev device implementation (via `qemu` submodule)
- A buildroot environment to run inside QEMU (target) for tests and development
  (via `buildroot` submodule)
- Tests and programs for the i2c chardev device for the host and target side
    - See `br2-external` for the buildroot external tree with custom packages
    - Host tools in `host-mctp-i2c-tools/` (echo and initiator)
- Documentation and examples for using the i2c chardev device

## Building

### Prerequisites

- [Task](https://taskfile.dev/) - Build automation tool
- Rust toolchain (for host MCTP I2C tools)
- Standard build tools (gcc, make, etc.)
- QEMU build dependencies

### Build Steps

1. **Initialize submodules**:
   ```bash
   task init-submodules
   ```

2. **Build QEMU**:
   ```bash
   task build-qemu
   ```
   Builds `qemu-system-arm` with the I2C chardev device support.

3. **Build Host MCTP I2C Tools**:
   ```bash
   task build-host-mctp-i2c-tools
   ```
   Builds the `echo` and `initiator` binaries in `host-mctp-i2c-tools/target/release/`.

4. **Build Buildroot**:
   ```bash
   task build-buildroot
   ```
   Builds the Linux kernel, rootfs, and device tree for the AST2600 EVB target.

**Note**: Building QEMU and Buildroot can take 10-20 minutes on a fast machine. The buildroot
build includes building a complete toolchain as the prebuild toolchain from Bootlin lacks
the kernel headers required for MCTP tooling.

### Clean Targets

- `task clean-qemu` - Clean QEMU build artifacts
- `task clean-buildroot` - Clean Buildroot build artifacts
- `task clean` - Clean all build artifacts

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
