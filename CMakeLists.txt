#
# Copyright (c) 2021 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(vibTrack)

# NORDIC SDK APP START
target_sources(app PRIVATE
  src/SystemStateMachine/SystemInit.c src/main.c src/BLE/ble.c src/Configurations/Configurations.c src/nrfconfig.c
)
# NORDIC  SDK APP END
