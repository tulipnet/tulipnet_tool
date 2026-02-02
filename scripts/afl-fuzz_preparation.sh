#!/bin/bash -e

echo core | sudo tee /proc/sys/kernel/core_pattern
pushd /sys/devices/system/cpu
echo performance | sudo tee cpu*/cpufreq/scaling_governor
popd
