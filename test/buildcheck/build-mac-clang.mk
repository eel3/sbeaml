# @brief   SBEAML: Makefile for checking the syntax (macOS clang)
# @author  eel3
# @date    2022-02-09

# ---------------------------------------------------------------------

PREFIX         := xcrun 
CC             := $(PREFIX)$(CC)

CFLAGS          =
LDFLAGS         =
LDLIBS         :=

CCDEFS          =
OBJADD         :=
WARNADD        :=
USE_ASSERT     :=

# ---------------------------------------------------------------------

SDKROOT        := $(shell xcodebuild -version -sdk macosx | sed -n '/^Path: /s///p')

CPPFLAGS       := -isysroot "$(SDKROOT)"
TARGET_ARCH    := -mmacosx-version-min=10.15 -arch x86_64 -arch arm64

# ---------------------------------------------------------------------

include ./build-common.mk
