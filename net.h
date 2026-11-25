#ifndef NET_H
#define NET_H


#include <stddef.h>
#include <stdint.h>

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

#define NET_DEVICE_TYPE_DUMMY     0x0000
#define NET_DEVICE_TYPE_LOOPBACK  0x0001
#define NET_DEVICE_TYPE_ETHERNET  0x0002

#define NET_DEVICE_FLAG_UP        0x0001
#define NET_DEVICE_FLAG_LOOPBACK  0x0010
#define NET_DEVICE_FLAG_BROADCAST 0x0020
#define NET_DEVICE_FLAG_P2P       0x0040
#define NET_DEVICE_FLAG_NEED_ARP  0x0100

#define NET_DEVICE_ADDR_LEN 16

#define NET_DEVICE_IS_UP(x) ((x)->flags & NET_DEVICE_FLAG_UP)
#define NET_DEVICE_STATE(x) (NET_DEVICE_IS_UP(x) ? "UP" : "DOWN")

#define NET_PROTOCOL_TYPE_IP    0x0800
#define NET_PROTOCOL_TYPE_ARP   0x0806
#define NET_PROTOCOL_TYPE_IPV6  0x86dd

// ネットワークデバイス構造体
struct net_device {
    struct net_device *next;    // 次のネットワークデバイス。終端はNULL
    unsigned int index;         // ネットワークデバイスのID
    char name[IFNAMSIZ];        // 名称
    uint16_t type;              // 種類(NET_DEVICE_TYPE_*)
    uint16_t mtu;               // MTU(Maximum Transfer Unit): 一度に送信できるデータサイズ
    uint16_t flags;             // 特性と状態(NET_DEVICE_FLAG_*)
    uint16_t hlen;              // ヘッダ長
    uint16_t alen;              // アドレス長
    uint8_t addr[NET_DEVICE_ADDR_LEN];      // ネットワークデバイスのアドレス
    uint8_t broadcast[NET_DEVICE_ADDR_LEN]; // データリンクのブロードキャストアドレス
    struct net_device_ops *ops; // 制御ルーチンのアドレス構造体
    void *priv;                 // 任意のデータを指すポインタ
};

// デバイスドライバのインタフェース
//   デバイスドライバに実装されている制御ルーチンのアドレスを格納
struct net_device_ops {
    // ネットワークデバイスを起動する
    int (*open)(struct net_device *dev);
    // ネットワークデバイスを停止する
    int (*close)(struct net_device *dev);
    // ネットワークデバイスにデータを出力する
    int (*output)(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst);
};

// プロトコルのパケットを処理する入力ハンドラ
typedef void (*net_protocol_handler_t)(const uint8_t *data, size_t len, struct net_device *dev);

// ネットワークデバイスのオブジェクトを割り当て
extern struct net_device *net_device_alloc(void);
// ネットワークデバイスをプロトコルスタックに登録
extern int net_device_register(struct net_device *dev);
// ネットワークデバイスからデータを送信
extern int net_device_output(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst);

// プロトコルをプロトコルスタックに登録
extern int net_protocol_register(uint16_t type, net_protocol_handler_t handler);
// デバイスドライバからプロトコルスタックへ入力パケットを渡す
extern int net_input(uint16_t type, const uint8_t *data, size_t len, struct net_device *dev);
// プロトコルスタックの初期化
extern int net_init(void);
// プロトコルスタックの起動
extern int net_run(void);
// プロトコルスタックの停止
extern int net_shutdown(void);

#endif
