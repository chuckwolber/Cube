VERSION=0.0.1

PROJECTS = order test
.PHONY: $(PROJECTS) all clean

all: $(PROJECTS)

order:
	$(MAKE) -C $@ all

test:
	$(MAKE) -C $@ all

clean:
	$(MAKE) -C test clean
	$(MAKE) -C order clean

