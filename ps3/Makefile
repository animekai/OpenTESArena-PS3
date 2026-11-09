.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "Define PSL1GHT")
endif
include $(PSL1GHT)/ppu_rules
.RECIPEPREFIX := >

TARGET    := etapa1
BUILD     := build
ENGINE    := ../juego-original
TITLE     := Arena PS3 Etapa1
APPID     := ARENA0001
CONTENTID := UP0001-$(APPID)_00-0000000000000000

SRCDIRS := source compat $(ENGINE)/OpenTESArena/src/Math $(ENGINE)/components/vfs $(ENGINE)/components/archives \
  $(ENGINE)/components/debug $(ENGINE)/components/utilities \
  $(ENGINE)/OpenTESArena/src/Assets $(ENGINE)/OpenTESArena/src/Utilities
CPPLIST := main.cpp ps3_stubs.cpp Random.cpp manager.cpp archive.cpp bsaarchive.cpp Debug.cpp Bytes.cpp \
  Directory.cpp File.cpp Path.cpp String.cpp IMGFile.cpp COLFile.cpp \
  Compression.cpp Color.cpp
INCLUDES := compat $(ENGINE) $(ENGINE)/OpenTESArena/src
LIBDIRS  := $(PS3DEV)/portlibs/ppu

CFLAGS   = -O2 -Wall -mcpu=cell $(MACHDEP) $(INCLUDE) -I$(PS3DEV)/portlibs/ppu/include/SDL2
CXXFLAGS = -std=gnu++20 -D_GNU_SOURCE -include ps3_compat.h $(CFLAGS)
LDFLAGS  = $(MACHDEP) -Wl,-Map,$(notdir $@).map
LIBS     := -lSDL2main -lSDL2 -lrsx -lgcm_sys -lio -laudio -lsysutil -lsysmodule -lrt -llv2 -lm

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT   := $(CURDIR)/$(TARGET)
export VPATH    := $(foreach d,$(SRCDIRS),$(CURDIR)/$(d))
export DEPSDIR  := $(CURDIR)/$(BUILD)
export BUILDDIR := $(CURDIR)/$(BUILD)
export LD       := $(CXX)
export OFILES   := $(CPPLIST:.cpp=.o)
export INCLUDE  := $(foreach d,$(INCLUDES),-I$(CURDIR)/$(d)) $(LIBPSL1GHT_INC) -I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach d,$(LIBDIRS),-L$(d)/lib) $(LIBPSL1GHT_LIB)
.PHONY: $(BUILD) clean
$(BUILD):
> @[ -d $@ ] || mkdir -p $@
> @$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
clean:
> @rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).self $(OUTPUT).fake.self
pkg: $(BUILD) $(OUTPUT).pkg
else
DEPENDS := $(OFILES:.o=.d)
$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
-include $(DEPENDS)
endif
