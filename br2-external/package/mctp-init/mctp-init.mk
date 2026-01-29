################################################################################
#
# mctp-init
#
################################################################################

MCTP_INIT_VERSION = 1.0
MCTP_INIT_SITE = $(BR2_EXTERNAL_I2C_INTEGRATION_PATH)/package/mctp-init/src
MCTP_INIT_SITE_METHOD = local

define MCTP_INIT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/S99mctp $(TARGET_DIR)/etc/init.d/S99mctp
endef

$(eval $(generic-package))
