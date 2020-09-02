</$objtype/mkfile

SRC=checkdisk.c

%.$O: %.c
	$CC $CFLAGS -o $target $prereq

$O.out: ${SRC:%.c=%.$O}
	$LD $LFLAGS -o $target $prereq

clean nuke:V:
	rm -f [$OS].out *.[$OS]

install:V: $O.out
	cp $O.out /$objtype/bin/disk/check

uninstall:V:
	rm -f /$objtype/bin/disk/check
