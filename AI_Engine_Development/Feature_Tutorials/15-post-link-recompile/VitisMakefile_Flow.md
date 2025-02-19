﻿<table class="sphinxhide" width="100%">
 <tr>
   <td align="center"><img src="https://raw.githubusercontent.com/Xilinx/Image-Collateral/main/xilinx-logo.png" width="30%"/><h1>Post-Link Recompile of an AI Engine Application</h1>
   </td>
 </tr>
 <tr>
 <td align="center"><h1>Lab 2: Vitis Makefile Flow</h1>
 </td>
 </tr>

</table>

# Initialization

All labs rely on a set of source files that are available in the `Files` sub-directory:

- `AIE` contains the source files for the AI Engine application.
- `AIE2` contains the source files for the AI Engine application with location constraints for one of the AI Engine kernels.
- `Constraints` contains the PLIO constraints file for the second phase.
- `data` contains data files used to verify application functionality.
- `HwLink` contains a system configuration file to link AI Engine and PL kernels.
- `PL` contains all PL kernels (`mm2s`, `s2mm`, and `polar_clip`).
- `PS` contains the host (A72) application that runs the PL kernels, launches the graph, and verifies the output.

Go to the lab directory:

```BASH
cd WithIntermediatePlatform
```

# Phase 1: Creating a Fixed Platform from an AI Engine Application and PL Kernels

This lab uses the same files as the [Vitis IDE flow](VitisIDE_Flow.md), but all the operations are completed within a terminal.

1. To complete phase 1, change directory: `cd Phase1`.

   This directory contains a single file, which is a Makefile. Open it in any editor or display it in the terminal using `more`, `less`, or `cat`. The Makefile contains three stages:

   * `kernels`: To build the PL kernels. The output is a number of XO files, which are packaged RTL kernels.

   * `aie`: To build the AI Engine graph. The output is the file `libadf.a`, which is the compiled graph with all the PL/AI Engine interfaces.

   * `link`: To link the AI Engine array design with the PL design. This stage creates the XCLBIN and the XSA files.

There are also optional stages to simulate the AI Engine application using the `aiesim` target and `run_emu` to launch hardware emulation that includes all the PL kernels.


The platform directory, `$PLATFORM_REPO_PATHS` is initialized in  `env_setup.h`. It should contain the platform **xilinx_vck190_base_202210_1**.

In the terminal, type `make clean phase1` and all the required stages (`kernels`, `aie`, and `link`) will be run in the terminal as well as hardware emulation (`package` and `run_emu`).

Check the placement of the AI Engine kernels using Vitis Analyzer and opening the `graph.aiecompile_summary` file.

**Note:** The hardware emulation does not launch automatically. You have to launch it manually.

The following is the last message in the terminal:

```
[  OK  ] Mounted /run/media/mmcblk0p1.
[  OK  ] Finished Record Runlevel Change in UTMP.

PetaLinux 2022.1_release_S03221922 versal-rootfs-common-20221 ttyAMA0

versal-rootfs-common-20221 login: root (automatic login)

[   69.227997] audit: type=1006 audit(1648486440.428:2): pid=596 uid=0 old-auid=4294967295 auid=0 tty=(none) old-ses=4294967295 ses=1 res=1
[   69.233487] audit: type=1300 audit(1648486440.428:2): arch=c00000b7 syscall=64 success=yes exit=1 a0=8 a1=fffff3d2d410 a2=1 a3=ffffba2fa6b0 items=0 ppid=1 pid=596 auid=0 uid=0 gid=0 euid=0 suid=0 fsuid=0 egid=0 sgid=0 fsgid=0 tty=(none) ses=1 comm="(systemd)" exe="/lib/systemd/systemd" key=(null)
[   69.234988] audit: type=1327 audit(1648486440.428:2): proctitle="(systemd)"
root@versal-rootfs-common-20221:~#

```

To launch the simulation, type:

```
cd /run/media/mmcblk0p1
./host.exe a.xclbin
```

At the end of the simulation the following message is displayed:

```
mm2s (DataIn1) completed with status(4)
[59672.228524] zocl-drm axi:zyxclmm_drm:  ffff0008003cd410 kds_del_context: Client pid(2424) del context CU(0x1)
polar_clip completed with status(4)
[59672.230697] zocl-drm axi:zyxclmm_drm:  ffff0008003cd410 kds_del_context: Client pid(2424) del context CU(0x2)
s2mm (DataOut1) completed with status(4)
[59674.351664] zocl-drm axi:zyxclmm_drm:  ffff0008003cd410 kds_del_context: Client pid(2424) del context CU(0xffffffff)
[59674.353188] zocl-drm axi:zyxclmm_drm:  ffff0008003cd410 kds_del_context: Client pid(2424) del context CU(0x0)
TEST PASSED
Releasing remaining XRT objects...
[59674.355157] [drm] bitstream 8ae06494-3226-bda4-94c7-906e136725ad unlocked, ref=0
[59674.452197] zocl-drm axi:zyxclmm_drm: zocl_destroy_client: client exits pid(2424)
root@versal-rootfs-common-20221:/run/media/mmcblk0p1#
```

You can get out of QEMU by hitting **Ctrl** + **a x**.




# Phase 2: Using a Platform Generated by Vitis and Modifying the AI Engine Application

Phase 2 contains the following stages:

 1. `pfm`: Create a fixed platform with the AI Engine and PS domains. The PS domain is a Linux-based OS that uses XRT to control the PL and the AI Engine kernels.

 2. `aie2`: As in the previous phase, this stage builds an AI Engine application, but in this case it uses the new platform constraints.

 3. `host`: Compile the PS application.

 4. `package2`: Create the PDI to load onto the device (`hw_emu`).

 5. `run_emu`: Launch the simulation and verifies that the output is still correct.


Platform creation is based on a base platform which is specified in the `env_setup.sh` file:

```BASH
export PFM_NAME=xilinx_vck190_base_202210_1
```

Generic Versal and specific platform directories are defined in the `Makefile`:

```Makefile
# Platform
VERSAL_SW := ${PLATFORM_DIR}/sw/versal/xilinx-versal-common-v2022.1
BASE_PLATFORM := ${PLATFORM_DIR}/$(PFM_NAME)/$(PFM_NAME).xpfm
PLATFORM_SW := ${PLATFORM_DIR}/$(PFM_NAME)/sw/$(PFM_NAME)

IMAGE   := ${PLATFORM_REPO_PATHS}/sw/versal/xilinx-versal-common-v2022.1/Image
```

The new platform is named `Phase1_container` and is located in the directory `platform_repo`:

```Makefile
# New platform
PFM := Phase1_container
# Phase 1 output
XCLBIN := ../Phase1/$(PFM).xclbin
XSA    := ../Phase1/$(PFM).xsa

# Phase 2 platform creation output
NEW_PLATFORM := ./platform_repo/$(PFM)/export/$(PFM)/$(PFM).xpfm
NEW_PLATFORM_SW := ./platform_repo/$(PFM)/export/$(PFM)/sw/$(PFM)

NEW_ROOTFS := $(NEW_PLATFORM_SW)/xrt/rootfs/rootfs.ext4
NEW_SYSROOT := $(NEW_PLATFORM_SW)/xrt/sysroot
```

The new platform is created by a Tcl file (`xsct_create_pfm.tcl`) run by `xsct`. This file is divided into multiple parts:

- Set the directory names based on the Versal software and VCK190 platform directory.
- Create the platform.
- Create and configure the AI Engine domain.
- Create and configure the A72 domain.
- Generate the platform based on all configurations.

The second stage, `make aie2`, consists of generating the AI Engine application graph. The kernel itself has not changed but a kernel location constraint has been added to the graph so that you can see a big difference in the kernel placement. Check this new placement using Vitis Analyzer on the compile summary.

The third stage, `make host`, is straightforward and you can see that the first object file that is created is `aie_control_xrt.o`. It is built from the CPP file that has been generated by the second stage.

The fourth stage, `make package2`, has a number of outputs. Among them is a file named `launch_hw_emu.sh`, which is used to run the hardware emulation in the last stage.

The fifth and final stage, `make run_emu`, starts the hardware emulation that will also verify the correctness of the output.


# Perform On-Board Testing

1. To perform on-board testing, the same stages can be replicated but with a different target:

- For phase 1, type `make TARGET=hw clean phase1`.
- For phase 2, type `make TARGET=hw phase2`.

2. When the phase 2 process is finished, an `sd_card.img` file has been created and can be used as in the [Direct Recompile Makefile](DirectRecompileMakefile_Flow.md) flow.

3. To launch the program on the board, type the following command:

```
cd /run/media/mmcblk0p1
./host.exe a.xclbin
```


### Support

GitHub issues will be used for tracking requests and bugs. For questions go to [forums.xilinx.com](http://forums.xilinx.com/).

### License

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.

You may obtain a copy of the License at [http://www.apache.org/licenses/LICENSE-2.0]( http://www.apache.org/licenses/LICENSE-2.0 )


Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

<p align="center"><sup>XD039 | &copy; Copyright 2021–2022 Xilinx, Inc.</sup></p>