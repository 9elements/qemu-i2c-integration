################################################################################
#
# mctp-echo
#
################################################################################

MCTP_ECHO_VERSION = 1.0
MCTP_ECHO_SITE = $(BR2_EXTERNAL_I2C_INTEGRATION_PATH)/package/mctp-echo/src
MCTP_ECHO_SITE_METHOD = local
MCTP_ECHO_LICENSE = GPL-2.0+
MCTP_ECHO_LICENSE_FILES = LICENSE

define MCTP_ECHO_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)
endef

define MCTP_ECHO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/mctp-echo $(TARGET_DIR)/usr/bin/mctp-echo
endef

$(eval $(generic-package))
