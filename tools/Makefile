#exclude_dirs:= . template
exclude_dirs:= . template
subdirs:=$(shell find . -maxdepth 1 -type d)
subdirs:=$(patsubst ./%,%,$(subdirs))
subdirs:=$(filter-out $(exclude_dirs),$(subdirs)) fpktools
all:
	@for i in ${subdirs} ;do \
		if [ ! -f $$i/Makefile ];then \
			if [ -f $$i/${gCONFIGURE_SH} ];then \
				cd $$i;echo "[`pwd`] ./${gCONFIGURE_SH}";./${gCONFIGURE_SH}||exit $?;cd ..; \
			elif [ -f $$i/configure ];then \
				cd $$i;echo "[`pwd`] ./configure";./configure||exit $?;cd ..; \
			fi; \
		fi; \
		if [ -f $$i/${gMAKEFILE_SH} ];then \
			cd $$i;echo "[`pwd`] ./${gMAKEFILE_SH}";./${gMAKEFILE_SH}||exit $?;cd ..; \
		elif [ -f $$i/Makefile ];then \
			echo "[`pwd`] make -C $$i $@";make -C $$i $@ || exit $?; \
		fi; \
	done
install clean distclean:
	@for i in ${subdirs} ;do \
		if [ -f $$i/Makefile ]; then \
			echo "[`pwd`] make -C $$i $@";make -C $$i $@; \
		fi; \
	done
.PHONY: all install clean distclean

