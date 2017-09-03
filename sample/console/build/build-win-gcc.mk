# @brief   SBEAML: Makefile for sample application (Windows MinGW/TDM-GCC)
# @author  eel3
# @date    2017-08-31

# ---------------------------------------------------------------------

PREFIX         :=
CC             := $(PREFIX)gcc

CFLAGS          =
LDFLAGS         =
LDLIBS         :=

CCDEFS          =
OBJADD         :=
WARNADD        :=
USE_ASSERT     :=

# ---------------------------------------------------------------------

include ./build-common.mk
