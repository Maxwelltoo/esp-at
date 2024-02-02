# if (DEFINED ENV{AT_SSCMA_SUPPORT})
set(require_components nvs_flash at mdns wifi_provisioning)

idf_component_register(SRCS at_sscma.c
                    PRIV_REQUIRES ${require_components})
# endif()