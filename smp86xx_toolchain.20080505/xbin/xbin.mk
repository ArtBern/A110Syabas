##
# Makefile to integrate xbin to the toolchain package

xbins := \
    zeropad.bash \
    revbytes.pl \
    mkxload.bash \
    buildxrpc.bash \
    frombin.bash \
    tobin.bash \
    xbin.bash \
    xos_xrpc.inc \
    xsdk_env.bash
xbin_dir         := $(BASE_DIR)/xbin
xbin_install_dir := $(STAGING_DIR)/bin
xbin_scripts     := $(xbins:%=$(xbin_dir)/%)
xbin_targets     := $(xbins:%=$(xbin_install_dir)/%)

$(xbin_targets): $(xbin_install_dir)/%: $(xbin_dir)/%
	mkdir -p $(@D)
	cp $< $@

.PHONY: xbin
xbin: $(xbin_targets)

.PHONY: xbin-clean
xbin-clean:
	-rm $(xbin_targets)

.PHONY: xbin-dirclean
xbin-dirclean: xbin-clean
