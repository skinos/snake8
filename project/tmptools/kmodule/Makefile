# =============================================================================
# Kbuild fragment — build mod.ko from module.c
# =============================================================================
# Usage (typical out-of-tree, from kernel build docs):
#   make -C $(KERNEL_SRC) M=$(PWD) modules
#
# To ship as mydriver.ko instead of mod.ko:
#   obj-m           := mydriver.o
#   mydriver-objs   := module.o
#   (or rename module.c → mydriver.c and set obj-m := mydriver.o)
# =============================================================================

OBJS := module.o

EXTRA_CFLAGS +=

mod-objs := $(OBJS)
obj-m := mod.o
