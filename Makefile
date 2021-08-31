# This Makefile doesn't perform any building on its own, it's rather a wrapper
# for CMake. The reason for it is to make possible building Hotkey Detective
# for multiple architectures at once automatically.

BUILD_TYPE = MinSizeRel
CMAKE_GENERATOR = Visual Studio 15 2017

ARCHS = x64 Win32
HKD_TARGETS = HotkeyDetective.exe hotkey_hook.dll

TARGETS = $(foreach arch,$(ARCHS),$(addprefix $(arch)/,$(HKD_TARGETS)))

VERSION := $(shell git tag --points-at origin/master)

.PHONY: release clean
.SECONDARY:
.SECONDEXPANSION:

%.exe: cmake_$$(word 1,$$(subst /, ,$$@))
	cp $^\$(BUILD_TYPE)\$(notdir $*).exe $@

%.dll: cmake_$$(word 1,$$(subst /, ,$$@))
	cp $^\dll\$(BUILD_TYPE)\$(notdir $*).dll $@

cmake_%:
	cmake -G "$(CMAKE_GENERATOR)" -A $* . -B $@
	cmake --build $@ --config $(BUILD_TYPE)

%.zip: README.adoc LICENSE $(TARGETS)
	zip -9 $@ $^

$(ARCHS):
	mkdir -p $@

clean:
	rm -rf $(ARCHS)
	rm -rf cmake_*
	rm -f hotkey-detective-*.zip

release: $(ARCHS) hotkey-detective-$(VERSION).zip