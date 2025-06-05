#include <zephyr/types.h>
#include <zephyr/device.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <bluetooth/services/nus.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include "BLE.h"

LOG_MODULE_REGISTER(LOG_MODULE_NAME);

const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(nordic_nus_uart));
struct k_work_delayable uart_work;
const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};
const size_t ad_len = ARRAY_SIZE(ad);
const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NUS_VAL),
};
const size_t sd_len = ARRAY_SIZE(sd);

void uart_work_handler(struct k_work *item)
{
	struct uart_data_t *buf;

	buf = k_malloc(sizeof(*buf));
	if (buf) {
		buf->len = 0;
	} else {
		LOG_WRN("Not able to allocate UART receive buffer");
		k_work_reschedule(&uart_work, UART_WAIT_FOR_BUF_DELAY);
		return;
	}

	uart_rx_enable(uart, buf->data, sizeof(buf->data), UART_WAIT_FOR_RX);
}