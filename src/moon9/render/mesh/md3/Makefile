SHELL = /bin/sh
SYSTEM = $(shell ./config.guess | cut -d - -f 3 | sed -e 's/[0-9\.]//g;')
SYSTEM.SUPPORTED = $(shell test -f Makefile.$(SYSTEM) && echo 1)

ifeq ($(SYSTEM.SUPPORTED), 1)
include Makefile.$(SYSTEM)
else
$(error "Platform '$(SYSTEM)' not supported")
endif
