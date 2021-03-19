#PROJECT Freethought Library, and FreeThought Interface

all:
	cd lib && make install
	cd ft_test && make
	cd fti && make

debug:
	cd lib && make debug
	cd ft_test && make debug
	cd fti && make debug

debug_install:
	cd lib && make debug_install
	cd ft_test && make debug_install
	cd fti && make debug_install

install:
	cd lib && make install
	cd ft_test && make install
	cd fti && make install

uninstall:
	cd lib && make uninstall
	cd ft_test && make uninstall
	cd fti && make uninstall

clean: 
	cd lib && make clean
	cd ft_test && make clean
	cd fti && make clean



