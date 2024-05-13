#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/sensor.h>
#define SLEEP_TIME_MS 2000
const struct device *mcp9808_dev = DEVICE_DT_GET_ANY(microchip_mcp9808);
static struct sensor_value temp;

static void mcp9808_work_cb(struct k_work *work)
{
    int err;
    if (!device_is_ready(mcp9808_dev))
    {
        printk("mcp9808_dev not ready\n");
        return;
    }
    do
    {
        err = sensor_sample_fetch(mcp9808_dev);
        if (err < 0)
        {
            printk("MCP9808 write failed: %d\n", err);
            break;
        }
        err = sensor_channel_get(mcp9808_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        if (err < 0)
        {
            printk("MCP9808 read failed: %d\n", err);
            break;
        }
        printk("MCP9808: %.2f Cel\n", sensor_value_to_double(&temp));
    } while (false);
}
K_WORK_DEFINE(mcp9808_work, mcp9808_work_cb);

void mytimer_cb(struct k_timer *dummy)
{
    k_work_submit(&mcp9808_work);
}

K_TIMER_DEFINE(mytimer, mytimer_cb, NULL);

int main(void)
{
    k_timer_start(&mytimer, K_SECONDS(2), K_SECONDS(2));
    while (true)
    {
        k_msleep(SLEEP_TIME_MS);
    }
}