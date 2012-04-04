all:
	(cd src; ${MAKE})

install:
	${MAKE} all
	(cd src; ${MAKE} install)

deinstall:
	(cd src; ${MAKE} deinstall}
