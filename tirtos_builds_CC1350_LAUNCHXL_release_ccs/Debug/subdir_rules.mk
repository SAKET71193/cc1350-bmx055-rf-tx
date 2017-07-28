################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
build-62969583:
	@$(MAKE) -Onone -f subdir_rules.mk build-62969583-inproc

build-62969583-inproc: ../release.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"/home/saket/Program/ti/xdctools_3_50_02_20_core/xs" --xdcpath="/home/saket/ti/simplelink_cc13x0_sdk_1_40_00_10/source;/home/saket/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages;/home/saket/Program/ti/ccsv7/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M3 -p ti.platforms.simplelink:CC1350F128 -r release -c "/home/saket/Program/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS" --compileOptions " -DDeviceFamily_CC13X0 " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-62969583 ../release.cfg
configPkg/compiler.opt: build-62969583
configPkg/: build-62969583


