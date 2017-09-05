# @brief   SBEAML: Makefile for sample application (Unix GCC)
# @author  eel3
# @date    2017-09-05

# ---------------------------------------------------------------------

PREFIX         :=
CC             := $(PREFIX)$(CC)

CFLAGS          =
LDFLAGS         = -pthread
LDLIBS         := -lstdc++

CCDEFS          =
OBJADD         :=
WARNADD        :=
USE_ASSERT     :=

# ---------------------------------------------------------------------

include ./build-common.mk
