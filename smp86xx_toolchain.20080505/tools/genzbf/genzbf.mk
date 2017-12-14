# Makefile for the genzbf utility

genzbf_binary        := genzbf
genzbf_dir           := $(BASE_DIR)/tools/genzbf
genzbf_install_dir   := $(STAGING_DIR)/bin

$(genzbf_dir)/$(genzbf_binary): FORCE
	$(MAKE) -C $(@D)

$(genzbf_install_dir)/$(genzbf_binary): $(genzbf_dir)/$(genzbf_binary)
	mkdir -p $(@D)
	cp $< $@

.PHONY: genzbf
genzbf: $(genzbf_install_dir)/$(genzbf_binary)

.PHONY: genzbf-clean
genzbf-clean:
	-rm $(genzbf_install_dir)/$(genzbf_binary)
	$(MAKE) -C $(genzbf_dir) clean

.PHONY: genzbf-dirclean
genzbf-dirclean: genzbf-clean
