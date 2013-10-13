#!/bin/sh

# --- C ---------------------------------------------------------------------

hostcq() {
	hostc "$SRCDIR/$1" "$OBJDIR/$1"
}

hostcdyn() {
	hostc "$OBJDIR/$1" "$OBJDIR/$1"
	addcleanable $OBJDIR/$1
}
	
hostc() {
	local srcroot="${1/%.?/}"
	local destroot="${2/%.?/}"

	CLEANABLE="$CLEANABLE $destroot.host.d"
	SRCS="$SRCS $srcroot.cc"
	OBJS="$OBJS $destroot.host.o"
cat <<EOF
-include $destroot.host.d
$destroot.host.o: $srcroot.c
	@echo -e "HOSTCC\t$destroot.host.o"
	@mkdir -p \$(dir \$@)
	@$HOSTCC $INCLUDES $DEFINES -c -o $destroot.host.o $srcroot.c
$destroot.host.d: $srcroot.c
	\$(call makedepend,\$@,\$<,$INCLUDES)
EOF
}

# --- C++ -------------------------------------------------------------------

hostccq() {
	hostcc "$SRCDIR/$1" "$OBJDIR/$1"
}

hostccdyn() {
	hostcc "$OBJDIR/$1" "$OBJDIR/$1"
	addcleanable $OBJDIR/$1
}
	
hostcc() {
	local srcroot="${1/%.??/}"
	local destroot="${2/%.??/}"

	CLEANABLE="$CLEANABLE $destroot.host.d"
	SRCS="$SRCS $srcroot.cc"
	OBJS="$OBJS $destroot.host.o"
cat <<EOF
-include $destroot.host.d
$destroot.host.o: $srcroot.cc
	@echo -e "HOSTCC\t$destroot.host.o"
	@mkdir -p \$(dir \$@)
	@$HOSTCC $INCLUDES $DEFINES -c -o $destroot.host.o $srcroot.cc
$destroot.host.d: $srcroot.cc
	\$(call makedepend,\$@,\$<,$INCLUDES)
EOF
}

# --- Linkage ---------------------------------------------------------------

hostprogram() {
	local binary="$1"

	local deps=""
	local bins=""
	shift
	while [ "$1" != "" ]; do
		bins="$bins $1"
		case "$1" in
			-l*)
				bins="$bins $1"
				;;

			*)
				deps="$deps $1"
				;;
		esac
		shift
	done

	BUILDABLE="$BUILDABLE $binary"
	CLEANABLE="$CLEANABLE $binary $deps"
cat <<EOF
$binary: $deps
	@echo -e "HOSTAPP\t$binary"
	@mkdir -p \$(dir \$@)
	@$HOSTCC -o $binary -L$LIBDIR $bins
EOF
}

hostlibrary() {
	local binary="$1"
	shift

	CLEANABLE="$CLEANABLE $binary $@"
cat <<EOF
$binary: $@
	@echo -e "HOSTLIB\t$binary"
	@mkdir -p \$(dir \$@)
	@rm -f $binary
	@ar cr $binary $@
EOF
}

