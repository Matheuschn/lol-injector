BUILDDIR := build
SOURCEDIR := src
LIBDIR := lib
UTILSDIR := utils
BINDIR := bin

CC := gcc
LDFLAGS := $(addprefix -L./, $(wildcard $(LIBDIR)/*/)) -lfrida-core -lssl -lcrypto -ldl
CFLAGS := -Wall -m32 -fuse-ld=gold -Wl,--icf=all,--gc-sections,-z,noexecstack \
		  -Os -s -ffast-math -pthread -ffunction-sections -fdata-sections \
		  -static-libgcc

SRC := $(foreach x, $(SOURCEDIR), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

default: all

launchhelper: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BINDIR)/$@ $(LDFLAGS) 

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

.PHONY: makedir
makedir:
	mkdir -p $(BUILDDIR) $(BINDIR)

.PHONY: all
all: makedir launchhelper

.PHONY: dist
dist: all
	$(UTILSDIR)/upx -9 $(BINDIR)/launchhelper

.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(BINDIR)
