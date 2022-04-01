BUILDDIR := build
SOURCEDIR := src
LIBDIR := lib
UTILSDIR := utils
BINDIR := bin

CC := gcc
LDFLAGS := -L./$(LIBDIR)/frida-core-x86 -lfrida-core -lssl
CFLAGS := -Wall -m32 -fuse-ld=gold -Wl,--icf=all,--gc-sections,-z,noexecstack \
		  -Os -s -ffast-math

SRC := $(foreach x, $(SOURCEDIR), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

default: makedir all

launchhelper: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BINDIR)/$@ $(LDFLAGS) 

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

.PHONY: makedir
makedir:
	mkdir -p $(BUILDDIR) $(BINDIR)

.PHONY: all
all: launchhelper

.PHONY: dist
dist: launchhelper
	$(UTILSDIR)/upx -9 $(BINDIR)/launchhelper

.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(BINDIR)
