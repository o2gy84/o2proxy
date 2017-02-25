all: world

world:
	@ make -C third_party/libproperty
	@ make -C third_party/o2logger
	@ make -C src
