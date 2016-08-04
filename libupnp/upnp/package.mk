#############################################################
#
# LIBUPNP
#
#############################################################


# Dependencies
LIBIUPNP_DEPENDENCIES=
#############################################################
### Version and change logs area
LIBIUPNP_VERSION=
define LIBIUPNP_CHANGELOGS

endef
#############################################################

# Program name
LIBIUPNP_PROGRAM_NAME=iupnp

# Target name
LIBIUPNP_TARGET=shared

LIBIUPNP_FEATURE=libupnp

LIBIUPNP_TYPE=opensrclib

LIBIUPNP_MAKEFILE_DIR=src

LIBIUPNP_INDEX=386

LIBIUPNP_LICENSE="BSD"

define LIBIUPNP_INSTALL_HEADER_CMDS
	rm -fr $(STAGING_DIR)/usr/include/libupnp; mkdir $(STAGING_DIR)/usr/include/libupnp; \
	$(foreach upnp_header, $(notdir $(wildcard $(3)/inc/*.h)), \
	ls -l $(3)/inc/$(upnp_header);\
	install -m 777 $(3)/inc/$(upnp_header) $(STAGING_DIR)/usr/include/libupnp;)\
	$(foreach upnp_header, $(notdir $(wildcard $(3)/src/inc/*.h)), \
	install -m 777 $(3)/src/inc/$(upnp_header) $(STAGING_DIR)/usr/include/libupnp;)
endef

$(eval $(call ATPTARGETS,package/atp/libraries/opensrc/libupnp/upnp,libiupnp,BUILD_LIBUPNP))
