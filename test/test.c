#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include "util.h"
#include "net.h"

#include "test.h"

static volatile sig_atomic_t terminate;

static struct net_device *dev;

struct net_device *dummy_init(void)
{
    struct net_device *dev;

    // # 1 #####################################
    dev = net_device_alloc();
    if (!dev) {
        errorf("net_device_alloc() failure");
        return NULL;
    }

    dev->type = NET_DEVICE_TYPE_DUMMY;
    dev->mtu = 128;
    dev->hlen = 0;
    dev->alen = 0;
    if (net_device_register(dev) == -1) {
        errorf("net_device_register() failure");
        return NULL;
    }

    // # 2 #####################################
    dev = net_device_alloc();
    if (!dev) {
        errorf("net_device_alloc() failure");
        return NULL;
    }

    dev->type = NET_DEVICE_TYPE_DUMMY;
    dev->mtu = 64;
    dev->hlen = 0;
    dev->alen = 0;
    if (net_device_register(dev) == -1) {
        errorf("net_device_register() failure");
        return NULL;
    }

    infof("success, dev=%s", dev->name);
    return dev;
}

static void on_signal(int signum)
{
    (void)signum;
    terminate = 1;
    return;
}

static int setup(void)
{
    struct sigaction sa = {0};  // {0}: 構造体のすべてのメンバを 0 で初期化

    // SIGINTを受信したらon_signalを呼ぶよう設定
    sa.sa_handler = on_signal;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        errorf("sigaction() %s", strerror(errno));
        return -1;
    }
    infof("setup protocol stack...");
    // プロトコルスタックの初期化
    if (net_init() == -1) {
        errorf("net_init() failure");
        return -1;
    }

    dev = dummy_init();
    if (!dev) {
        errorf("dummy_init() failure");
        return -1;
    }

    // プロトコルスタックの起動
    if (net_run() == -1) {
        errorf("net_run() failure");
        return -1;
    }
    return 0;
}

static int cleanup(void)
{
    infof("cleanup protocol stack...");
    if (net_shutdown() == -1) {
        errorf("net_run() failure");
        return -1;
    }
    return 0;
}

static int app_main(void)
{
    struct net_device *working_dev;

    debugf("press Ctrl+C to terminate");
    while (!terminate) {
        for (working_dev = dev; working_dev; working_dev = working_dev->next) {
            if (net_device_output(working_dev, 0x0800, test_data, sizeof(test_data), NULL) == -1) {
                errorf("net_device_output() failure");
                break;
            }
        }
        sleep(1);
    }
    debugf("terminate");
    return 0;
}

int main(void)
{
    int ret;

    if (setup() == -1) {
        errorf("setup() failure");
        return -1;
    }
    ret = app_main();
    if (cleanup() == -1) {
        errorf("cleanup() failure");
        return -1;
    }
    return ret;
}
