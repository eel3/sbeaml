# @brief   SBEAML: Makefile for sample application (Unix environment)
# @author  eel3
# @date    2017-09-02

# ---------------------------------------------------------------------

root-dir       := ../../..

src-dir        := $(root-dir)/src
include-dir    := $(src-dir)/include
lib-dir        := $(src-dir)/lib
machdep-dir    := $(src-dir)/machdep
md-sample-dir  := $(machdep-dir)/sample

app-dir        := ..

#----------------------------------------------------------------------

VPATH          := $(lib-dir) $(app-dir)

include-dirs   := $(addprefix -I , \
                  $(include-dir) \
                  $(md-sample-dir) \
                  $(VPATH))

object-files   := sbeaml.o \
                  sbeaml_md.o \
                  main.o \
                  handler_common.o \
                  handler_inner.o \
                  handler_leaf.o \
                  handler_root.o
depend-files   := $(subst .o,.d,$(object-files))

target-orig-name   := main
target-name        := console

#----------------------------------------------------------------------

ifdef USE_ASSERT
CCDEFS     += -DDEBUG
else
CCDEFS     += -DNDEBUG
endif

CCDEFS     +=
OPTIM      ?= -O0
WARN       ?= -Wall -pedantic \
              -Wextra \
              -Wunused-result \
              -Wno-unused-function -Wcast-align \
                  -Wmissing-include-dirs -Wundef \
                  -Werror-implicit-function-declaration \
              # -Wno-long-long
CWARN      ?= -std=c89 $(WARN) -Wbad-function-cast
CXXWARN    ?= -std=c++11 $(WARN)

CFLAGS     += $(OPTIM) $(CWARN) $(WARNADD)
CXXFLAGS   += $(OPTIM) $(CXXWARN) $(WARNADD)
CPPFLAGS   += $(CCDEFS) $(include-dirs)
LDFLAGS    += $(OPTIM)

#----------------------------------------------------------------------

phony-targets  := all clean usage

.PHONY: $(phony-targets)

usage:
	# $(MAKE) -f build-<target-arch>.mk $(patsubst %,[%],$(phony-targets))

all: $(target-name)

$(target-name): $(target-orig-name)
	cp -fp $< $@

$(target-orig-name): $(object-files)

clean:
	$(RM) $(target-name) $(target-orig-name) $(object-files) $(depend-files)

#----------------------------------------------------------------------

ifneq "$(MAKECMDGOALS)" ""
ifneq "$(MAKECMDGOALS)" "clean"
ifneq "$(MAKECMDGOALS)" "usage"
  -include $(depend-files)
endif
endif
endif

# $(call make-depend,source-file,object-file,depend-file,flags)
make-depend = $(CC) -MM -MF $3 -MP -MT $2 $4 $(CPPFLAGS) $1

%.o: %.c
	$(call make-depend,$<,$@,$(subst .o,.d,$@),$(CFLAGS))
	$(COMPILE.c) $(OUTPUT_OPTION) $<

%.o: %.cpp
	$(call make-depend,$<,$@,$(subst .o,.d,$@),$(CXXFLAGS))
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
