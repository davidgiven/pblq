define makedepend
    @mkdir -p $(dir $(1)); \
        set -e; $(CC) -MM $(CPPFLAGS) $(4) $(3) $(2) 2>/dev/null \
                | sed 's|$(basename $(notdir $(2)))\.o[ :]*|$(basename $(1)).o $(1) : |g' > $(1); \
                [ -s $(1) ] || rm -f $(1)
endef

default: all

