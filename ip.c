#include <stdint.h>
#include <stddef.h>

#include "util.h"
#include "net.h"
#include "ip.h"

/**
 * IPモジュールの入力ハンドラ
 *
 * data: パケットのバイト列
 * len:  パケットの長さ
 * dev:  入力元のネットワークデバイス
 */
static void ip_input(const uint8_t *data, size_t len, struct net_device *dev)
{
    debugf("dev=%s, len=%zu", dev->name, len);
    debugdump(data, len);
}

// IPモジュールの初期化
int ip_init(void)
{
    if (net_protocol_register(NET_PROTOCOL_TYPE_IP, ip_input) == -1) {
        errorf("net_protocol_register() failure");
        return -1;
    }
    return 0;
}
