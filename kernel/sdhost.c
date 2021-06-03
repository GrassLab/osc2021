#include "sdhost.h"

static inline void delay(unsigned long tick) {
  while (tick--) {
    asm volatile("nop");
  }
}

static int is_hcs;  // high capcacity(SDHC)

static void pin_setup() {
  set(GPIO_GPFSEL4, 0x24000000);
  set(GPIO_GPFSEL5, 0x924);
  set(GPIO_GPPUD, 0);
  delay(15000);
  set(GPIO_GPPUDCLK1, 0xffffffff);
  delay(15000);
  set(GPIO_GPPUDCLK1, 0);
}

static void sdhost_setup() {
  unsigned int tmp;
  set(SDHOST_PWR, 0);
  set(SDHOST_CMD, 0);
  set(SDHOST_ARG, 0);
  set(SDHOST_TOUT, SDHOST_TOUT_DEFAULT);
  set(SDHOST_CDIV, 0);
  set(SDHOST_HSTS, SDHOST_HSTS_MASK);
  set(SDHOST_CFG, 0);
  set(SDHOST_CNT, 0);
  set(SDHOST_SIZE, 0);
  get(SDHOST_DBG, tmp);
  tmp &= ~SDHOST_DBG_MASK;
  tmp |= SDHOST_DBG_FIFO;
  set(SDHOST_DBG, tmp);
  delay(250000);
  set(SDHOST_PWR, 1);
  delay(250000);
  set(SDHOST_CFG, SDHOST_CFG_SLOW | SDHOST_CFG_INTBUS | SDHOST_CFG_DATA_EN);
  set(SDHOST_CDIV, SDHOST_CDIV_DEFAULT);
}

static int wait_sd() {
  int cnt = 1000000;
  unsigned int cmd;
  do {
    if (cnt == 0) {
      return -1;
    }
    get(SDHOST_CMD, cmd);
    --cnt;
  } while (cmd & SDHOST_NEW_CMD);
  return 0;
}

static int sd_cmd(unsigned cmd, unsigned int arg) {
  set(SDHOST_ARG, arg);
  set(SDHOST_CMD, cmd | SDHOST_NEW_CMD);
  return wait_sd();
}

static int sdcard_setup() {
  unsigned int tmp;
  sd_cmd(GO_IDLE_STATE | SDHOST_NO_REPONSE, 0);
  sd_cmd(SEND_IF_COND, VOLTAGE_CHECK_PATTERN);
  get(SDHOST_RESP0, tmp);
  if (tmp != VOLTAGE_CHECK_PATTERN) {
    return -1;
  }
  while (1) {
    if (sd_cmd(APP_CMD, 0) == -1) {
      // MMC card or invalid card status
      // currently not support
      continue;
    }
    sd_cmd(SD_APP_OP_COND, SDCARD_3_3V | SDCARD_ISHCS);
    get(SDHOST_RESP0, tmp);
    if (tmp & SDCARD_READY) {
      break;
    }
    delay(1000000);
  }

  is_hcs = tmp & SDCARD_ISHCS;
  sd_cmd(ALL_SEND_CID | SDHOST_LONG_RESPONSE, 0);
  sd_cmd(SEND_RELATIVE_ADDR, 0);
  get(SDHOST_RESP0, tmp);
  sd_cmd(SELECT_CARD, tmp);
  sd_cmd(SET_BLOCKLEN, 512);
  return 0;
}

static int wait_fifo() {
  int cnt = 1000000;
  unsigned int hsts;
  do {
    if (cnt == 0) {
      return -1;
    }
    get(SDHOST_HSTS, hsts);
    --cnt;
  } while ((hsts & SDHOST_HSTS_DATA) == 0);
  return 0;
}

static void set_block(int size, int cnt) {
  set(SDHOST_SIZE, size);
  set(SDHOST_CNT, cnt);
}

static void wait_finish() {
  unsigned int dbg;
  do {
    get(SDHOST_DBG, dbg);
  } while ((dbg & SDHOST_DBG_FSM_MASK) != SDHOST_HSTS_DATA);
}

void readblock(int block_idx, void* buf) {
  unsigned int* buf_u = (unsigned int*)buf;
  int succ = 0;
  if (!is_hcs) {
    block_idx <<= 9;
  }
  do{
    set_block(512, 1);
    sd_cmd(READ_SINGLE_BLOCK | SDHOST_READ, block_idx);
    for (int i = 0; i < 128; ++i) {
      wait_fifo();
      get(SDHOST_DATA, buf_u[i]);
    }
    unsigned int hsts;
    get(SDHOST_HSTS, hsts);
    if (hsts & SDHOST_HSTS_ERR_MASK) {
      set(SDHOST_HSTS, SDHOST_HSTS_ERR_MASK);
      sd_cmd(STOP_TRANSMISSION | SDHOST_BUSY, 0);
    } else {
      succ = 1;
    }
  } while(!succ);
  wait_finish();
}

void writeblock(int block_idx, void* buf) {
  unsigned int* buf_u = (unsigned int*)buf;
  int succ = 0;
  if (!is_hcs) {
    block_idx <<= 9;
  }
  do{
    set_block(512, 1);
    sd_cmd(WRITE_SINGLE_BLOCK | SDHOST_WRITE, block_idx);
    for (int i = 0; i < 128; ++i) {
      wait_fifo();
      set(SDHOST_DATA, buf_u[i]);
    }
    unsigned int hsts;
    get(SDHOST_HSTS, hsts);
    if (hsts & SDHOST_HSTS_ERR_MASK) {
      set(SDHOST_HSTS, SDHOST_HSTS_ERR_MASK);
      sd_cmd(STOP_TRANSMISSION | SDHOST_BUSY, 0);
    } else {
      succ = 1;
    }
  } while(!succ);
  wait_finish();
}

void sd_init() {
  pin_setup();
  sdhost_setup();
  sdcard_setup();
}
