# ----------------------------
# Makefile Options
# ----------------------------

NAME = TEST2
DESCRIPTION = "GENECES - RIGHTS TO EVANG"
COMPRESSED = NO
BSSHEAP_LOW = D00000

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz -pedantic -Wfatal-errors -Wc++17-extensions -std=c++17
CXX_STANDARD = 17

# ----------------------------

include $(shell cedev-config --makefile)