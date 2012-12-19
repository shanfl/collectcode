SUBDIRS	=	 \
			glpng/src \
			src

all: $(SUBDIRS)

$(SUBDIRS): FORCE
	cd $@; $(MAKE)

clean:
	-rm -f ./bin/flow
	-rm -f *~ core
	-rm -f *.bck
	for i in $(SUBDIRS); do ( cd $$i ; $(MAKE) clean ) ; done

FORCE:
