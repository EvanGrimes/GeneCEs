# ----------------------------
# Makefile Options
# ----------------------------

NAME = Test2
DESCRIPTION = "CE C Toolchain Demo"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
