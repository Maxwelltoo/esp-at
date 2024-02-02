


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_at.h"
#include "esp_at_core.h"
#include "mdns.h"

#define CHECK_PARAMS_NUM(cnt, total)                \
    if (cnt != total) {                             \
        return ESP_AT_RESULT_CODE_ERROR;            \
    }

static uint8_t at_setup_cmd_mdns_add(uint8_t para_num)
{
    int32_t cnt = 0;
    uint8_t *item_name = NULL;
    uint8_t *item_data = NULL;

    if (esp_at_get_para_as_str(cnt++, &item_name) == ESP_AT_PARA_PARSE_RESULT_FAIL) {
        return ESP_AT_RESULT_CODE_ERROR;
    }
    if (esp_at_get_para_as_str(cnt++, &item_data) == ESP_AT_PARA_PARSE_RESULT_FAIL) {
        return ESP_AT_RESULT_CODE_ERROR;
    }

    CHECK_PARAMS_NUM(cnt, para_num);

    mdns_service_txt_item_set("_sscma", "_tcp", (char *)item_name, (char *)item_data);
    // mdns_txt_item_t serviceTxtData[1] = {
    //     {"board","esp32"},
    // };
    // // 设置服务的文本数据（会释放并替换当前数据）
    // mdns_service_txt_set("_sscma", "_tcp", serviceTxtData, 1);
    // // 修改服务端口号
    // mdns_service_port_set("_sscma", "_tcp", 4321);
    return ESP_AT_RESULT_CODE_OK;
}

static uint8_t at_setup_cmd_mdns_start(uint8_t para_num)
{
    int32_t cnt = 0;
    uint8_t *host_name = NULL;
    // uint8_t *instance_name = NULL;
    printf("at_setup_cmd_mdns_start\n");

    esp_err_t err = mdns_init();
    if (err) {
        printf("MDNS Init failed: %d\n", err);
        return ESP_AT_RESULT_CODE_ERROR;
    }

    if (esp_at_get_para_as_str(cnt++, &host_name) == ESP_AT_PARA_PARSE_RESULT_FAIL) {
        return ESP_AT_RESULT_CODE_ERROR;
    }
    CHECK_PARAMS_NUM(cnt, para_num);

    // 设置 hostname
    mdns_hostname_set((char *)host_name);
    // 设置默认实例：仅一个，等于 host name
    mdns_instance_name_set((char *)host_name);
    // 设置 service
    mdns_service_add((char *)host_name, "_sscma", "_tcp", 3141, NULL, 0);

    return ESP_AT_RESULT_CODE_OK;
}

static uint8_t at_test_cmd_mdns_start(uint8_t *cmd_name) {
    printf("at_test_cmd_mdns_start\n");
    // 初始化 mDNS 服务
    esp_err_t err = mdns_init();
    if (err) {
        printf("MDNS Init failed: %d\n", err);
        return ESP_AT_RESULT_CODE_ERROR;
    }
    // 设置 hostname
    mdns_hostname_set("grove_vision_ai_we2");
    // 设置默认实例：仅一个，等于 host name
    mdns_instance_name_set("grove_vision_ai_we2");
    //structure with TXT records
    mdns_txt_item_t serviceTxtData[] = {
        {"server","pcb511d8.ala.cn-hangzhou.emqxsl.cn"},
        {"protocol","mqtts"},
        {"port","8883"},
        {"device_id", "grove_vision_ai_we2_f433c150"}
    };
    //initialize service
    mdns_service_add("grove_vision_ai_we2", "_sscma", "_tcp", 3141, serviceTxtData, 4);
    // 添加服务
    // mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
    // 注意：必须先添加服务，然后才能设置其属性
    // mdns_service_instance_name_set("_http", "_tcp", "Jhon's ESP32 Web Server");
    // //add mac key string text item
    // mdns_service_txt_item_set("_modbus", "_tcp", "mac", gen_mac_str(sta_mac, "\0", temp_str));
    // //add slave id key txt item
    // mdns_service_txt_item_set("_modbus", "_tcp", "mb_id", gen_id_str("\0", temp_str));
    // mdns_free();
    return ESP_AT_RESULT_CODE_OK;
}

static const esp_at_cmd_struct s_at_sscma_cmd[] = {
    {"+MDNS_ADD", NULL, NULL, at_setup_cmd_mdns_add, NULL},
    {"+MDNS_START", at_test_cmd_mdns_start, NULL, at_setup_cmd_mdns_start, NULL},
};

bool esp_at_sscma_cmd_regist(void)
{
    return esp_at_custom_cmd_array_regist(s_at_sscma_cmd, sizeof(s_at_sscma_cmd) / sizeof(s_at_sscma_cmd[0]));
}