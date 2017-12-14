##
# Makefile to integrate genxenv into the toolchain package

genxenv             := genxenv
genxenv_dir         := $(BASE_DIR)/tools/genxenv
genxenv_install_dir := $(STAGING_DIR)/bin

genxenv_bin         := $(genxenv_dir)/$(genxenv)
genxenv_target      := $(genxenv_install_dir)/$(genxenv)

$(genxenv_bin): FORCE
# Unset RMCFLAGS to avoid trying to build genxenv for the target when the
# toolchain is not even ready yet.
	RMCFLAGS="" COMPILKIND=release $(MAKE) -C $(@D)

$(genxenv_target): $(genxenv_bin)
	mkdir -p $(@D)
	cp $< $@

.PHONY: genxenv
genxenv: $(genxenv_target)

.PHONY: genxenv-clean
genxenv-clean:
	-rm $(genxenv_target)
	$(MAKE) -C $(genxenv_dir) clean

.PHONY: genxenv-dirclean
genxenv-dirclean: genxenv-clean
