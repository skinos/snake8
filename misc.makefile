
# Ubuntu compile tools install
preset:
	sudo apt-get update
	sudo apt-get install -y vim git subversion pandoc aptitude tftp-hpa device-tree-compiler uuid-dev
	sudo apt-get install -y gcc g++ binutils patch bzip2 flex bison make autoconf gettext texinfo unzip sharutils gawk
	sudo apt-get install -y python3 libtool shtool curl lrzsz zlib1g-dev automake automake1.11 gcc-aarch64-linux-gnu
	sudo apt-get install -y lib32z1 libssl-dev device-tree-compiler libevent-dev libreadline-dev libmosquitto-dev libncurses-dev libevent-dev
	sudo apt-get install -y snapd wireguard-tools ripgrep
	sudo snap install ttyd --classic
	-sudo apt-get install -y lib32ncurses5
	-sudo apt-get install -y libncurses5-dev
	-sudo apt-get install -y libncursesw5-dev
	# Disable Check the SSL certificate of the server
	git config --global http.sslVerify false
.PHONY: preset

gCLANGD_GEN := ${gTOP_DIR}/tools/clangd_all.py
# IDE jump index: all intermediate files under .map/ (+ tiny root .clangd)
# Delete anytime: make map-clean   or   rm -rf .map .clangd
map:
	@echo "[map] generating .map/ compile databases + .clangd ..."
	@python3 "${gCLANGD_GEN}" "${gTOP_DIR}"
map-clean:
	@rm -rf "${gTOP_DIR}/.map" "${gTOP_DIR}/.clangd" "${gTOP_DIR}/compile_commands.json"
	@echo "[map] removed .map/ .clangd compile_commands.json"
.PHONY: map map-clean

# SDK Download
update:
	@set -e; \
	git pull; \
	if [ -d ${gPLATFORM_DIR} ]; then \
		echo "cd ${gPLATFORM_DIR}; git pull"; \
	else \
		echo "cd ${gTOP_DIR}/config; git clone git@gitee.com:snake8/${gPLATFORM}"; \
	fi; \
	$(MAKE) -f target.makefile sdk_update; \
	$(MAKE) -f target.makefile sdk_adjust; \
	$(MAKE) -f target.makefile sdk_menu

# SDK rebuild for HOST
rebuild:
	make;make sdk_install;make sdk_start
# SDK help for smtk2/smtk3
menu:
	make sdk_menu
menuconfig:
	make sdk_menuconfig

tftp:
	make sdk_tftp
ftp:
	make sdk_ftp
sz:
	make sdk_sz

stop:
	make sdk_stop
start:
	make sdk_start
install:
	make sdk_install

.PHONY: update rebuild menu menuconfig tftp ftp sz stop start install
