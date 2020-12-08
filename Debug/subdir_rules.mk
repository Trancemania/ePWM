################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="/opt/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="/home/trancemania/ti/C2000Ware_3_03_00_00_Software/device_support/f2833x/headers/include" --include_path="/home/trancemania/ti/C2000Ware_3_03_00_00_Software/device_support/f2833x/common/include" --include_path="/home/trancemania/ti/C2000Ware_3_03_00_00_Software/libraries/math/IQmath/c28/include" --include_path="/home/trancemania/ti/C2000Ware_3_03_00_00_Software/libraries/math/FPUfastRTS/c28/include" --define=_DEBUG --define=LARGE_MODEL -g --diag_suppress=10063 --diag_warning=225 --issue_remarks --verbose_diagnostics --quiet --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="/opt/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="/home/trancemania/ti/C2000Ware_3_03_00_00_Software/device_support/f2833x/headers/include" --include_path="/home/trancemania/ti/C2000Ware_3_03_00_00_Software/device_support/f2833x/common/include" --include_path="/home/trancemania/ti/C2000Ware_3_03_00_00_Software/libraries/math/IQmath/c28/include" --include_path="/home/trancemania/ti/C2000Ware_3_03_00_00_Software/libraries/math/FPUfastRTS/c28/include" --define=_DEBUG --define=LARGE_MODEL -g --diag_suppress=10063 --diag_warning=225 --issue_remarks --verbose_diagnostics --quiet --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


