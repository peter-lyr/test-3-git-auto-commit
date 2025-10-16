/*
*
*   HUART DMA后台DUMP RAM数据，使用工具bluetrum_voice_record_v1.3.4.exe
*
*/

#include "include.h"

#if HUART_DUMP_EN

#define HUART_DMUP_CH_MAX         2       //最多同时DUMP几路数据
#define HUART_DUMP_TX_SIZE        8
#define HUART_DUMP_TX_MASK        (HUART_DUMP_TX_SIZE - 1)
#define FRAME_SIZE_MAX_SIZE       256

struct tx_param_t {
    uint8_t *buf;
    uint len;
};

struct {
    struct tx_param_t tx[HUART_DUMP_TX_SIZE];
    u8 rptr;
    u8 wptr;
    volatile u8 len;
    volatile u8 tx_busy;
    u8 frame_cnt[6];            //最多能6路数据
    u8 toggle;
} huart_dump;

static u8 huart_dump_header[6][8];
static u8 huart_dump_buf[HUART_DMUP_CH_MAX][FRAME_SIZE_MAX_SIZE];

AT(.com_text.huart.dump)
void huart_dump_tx_done(void)
{
    huart_dump.tx_busy = 0;
    if (huart_dump.len) {
        huart_tx(huart_dump.tx[huart_dump.rptr & HUART_DUMP_TX_MASK].buf, huart_dump.tx[huart_dump.rptr & HUART_DUMP_TX_MASK].len);
        huart_dump.tx_busy = 1;
        huart_dump.rptr++;
        huart_dump.len--;
    }
}

AT(.com_text.huart.dump)
static bool huart_dump_putcs_do(uint8_t *buf, uint len)
{
    GLOBAL_INT_DISABLE();
    if (!huart_dump.tx_busy) {
        huart_tx(buf, len);
        huart_dump.tx_busy = 1;
    } else {
        if (huart_dump.len >= HUART_DUMP_TX_SIZE) {
            GLOBAL_INT_RESTORE();
            return false;
        }
        huart_dump.tx[huart_dump.wptr & HUART_DUMP_TX_MASK].buf = buf;
        huart_dump.tx[huart_dump.wptr & HUART_DUMP_TX_MASK].len = len;
        huart_dump.wptr++;
        huart_dump.len++;
    }
    GLOBAL_INT_RESTORE();
    return true;
}

AT(.com_text.huart.dump)
bool huart_dump_putcs(u8 type, u8 frame_num, void *buf, uint len)
{
    bool ret;
    huart_dump_header[type][4] = type;
    huart_dump_header[type][5] = (u8)(len >> 8);
    huart_dump_header[type][6] = (u8)(len >> 0);
    huart_dump_header[type][7] = frame_num;
    ret = huart_dump_putcs_do(huart_dump_header[type], 8);
    if (!ret) {
        return false;
    }
    ret = huart_dump_putcs_do(buf, len);
    return ret;
}

void huart_dump_init(void)
{
    memset(&huart_dump, 0, sizeof(huart_dump));
    memset(huart_dump_header, 0, 48);
    for(int i = 0; i < 6; i++){
        huart_dump_header[i][0] = 0x36;
        huart_dump_header[i][1] = 0xAD;
        huart_dump_header[i][2] = 0xf9;
        huart_dump_header[i][3] = 0x54;
    }
}

//dump一路数据
AT(.com_text.huart.dump)
void huart_dump_tx(u32 ch, u32 index, u8 *ptr, u32 size)
{
    u8 *dptr;

    if (ch >= HUART_DMUP_CH_MAX) {
        ch = 0;
    }
    if (size > FRAME_SIZE_MAX_SIZE) {
        size = FRAME_SIZE_MAX_SIZE;
    }
    dptr = huart_dump_buf[ch];
    memcpy(dptr, ptr, size);
    huart_dump_putcs(index, huart_dump.frame_cnt[index]++, dptr, size);
}

#endif //HUART_DUMP_EN
