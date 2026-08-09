exclude_dirs:= .
subdirs:=$(shell find . -maxdepth 1 -type d)
subdirs:=$(patsubst ./%,%,$(subdirs))
subdirs:=$(filter-out $(exclude_dirs),$(subdirs))
ifeq ("X${OBJ}","X")
  ifneq ("X${COMPILE_PROJECT}","X")
    PROJECTS := ${COMPILE_PROJECT}
    subdirs := ${COMPILE_PROJECT}
  else
    -include ${gPROJECT_CFGFILE}
  endif
else
PROJECTS := ${OBJ}
subdirs := ${OBJ}
endif
all dep install:
	for i in ${PROJECTS} ;do \
		if [ -d ${gPLATFORM_DIR}/$$i ]; then \
			if [ -f ${gPLATFORM_DIR}/$$i/${gPROJECT_INF} ]; then \
				if [ -f ${gPLATFORM_DIR}/$$i/fpk.makefile ]; then \
					echo "[`pwd`] make -f ${gPLATFORM_DIR}/$$i/fpk.makefile -C $$i $@"; \
					make -f ${gPLATFORM_DIR}/$$i/fpk.makefile -C ${gPLATFORM_DIR}/$$i $@ || exit $?; \
				else \
					echo "[`pwd`] make -f ${gFPK_MAKEFILE} -C $$i $@"; \
					make -f ${gFPK_MAKEFILE} -C ${gPLATFORM_DIR}/$$i $@ || exit $?; \
				fi; \
			fi; \
		elif [ -d ${gPROJECT_DIR}/$$i ]; then \
			if [ -f ${gPROJECT_DIR}/$$i/${gPROJECT_INF} ]; then \
				if [ -f ${gPROJECT_DIR}/$$i/fpk.makefile ]; then \
					echo "[`pwd`] make -f ${gPROJECT_DIR}/$$i/fpk.makefile -C $$i $@"; \
					make -f ${gPROJECT_DIR}/$$i/fpk.makefile -C ${gPROJECT_DIR}/$$i $@ || exit $?; \
				else \
					echo "[`pwd`] make -f ${gFPK_MAKEFILE} -C $$i $@"; \
					make -f ${gFPK_MAKEFILE} -C ${gPROJECT_DIR}/$$i $@ || exit $?; \
				fi; \
			fi; \
		elif [ -d ${gRICE_DIR}/$$i ]; then \
			if [ -f ${gRICE_DIR}/$$i/${gPROJECT_INF} ]; then \
				if [ -f ${gRICE_DIR}/$$i/fpk.makefile ]; then \
					echo "[`pwd`] make -f ${gRICE_DIR}/$$i/fpk.makefile -C $$i $@"; \
					make -f ${gRICE_DIR}/$$i/fpk.makefile -C ${gRICE_DIR}/$$i $@ || exit $?; \
				else \
					echo "[`pwd`] make -f ${gFPK_MAKEFILE} -C $$i $@"; \
					make -f ${gFPK_MAKEFILE} -C ${gRICE_DIR}/$$i $@ || exit $?; \
				fi; \
			fi; \
		fi; \
	done 
clean distclean:
	for i in ${subdirs} ;do \
		if [ -d ${gBUILD_DIR}/$$i ]; then \
			rm -fr ${gBUILD_DIR}/$$i; \
		fi; \
	done 
.PHONY: dep all install clean distclean

