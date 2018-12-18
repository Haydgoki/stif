# src makefile

BASE   = ..
COMMON = ${BASE}/common.mk
SRC    = $(wildcard *.c)
HDR    = $(wildcard ${BASE}/include/*.h)
OBJDIR = ${BASE}/obj/stif
OBJ    = $(addprefix ${OBJDIR}/, $(patsubst %.c,%.o,$(SRC)))

include ${COMMON}

CFLAGS += -I${BASE}/include


#CFLAGS += --coverage -O0  # rajouté pour le cov
#LDFLAGS += -fprofile-arcs -ftest-coverage  # rajouté pour le cov

all: ${OBJ}

${OBJDIR}/%.o: %.c ${HDR} ${COMMON}
	@mkdir -p ${OBJDIR}
	@echo "[*] Building $@"
	${CC} -o $@ -c $< ${CFLAGS} ${LDFLAGS}

clean:
	@echo "[*] Cleaning"
	rm -rf ${LIB} ${OBJ} ${COVDIR}
