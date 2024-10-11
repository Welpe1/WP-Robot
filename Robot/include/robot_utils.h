#ifndef __ROBOT_UTILS_H
#define __ROBOT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define TRY_IF_FAILED(func, result, count)                           \  
   do {                                                              \  
       uint8_t num = count;                                          \  
       while (num-- > 0) {                                           \  
           result = (func);                                          \  
           if (result == 0) { /* 0表示成功 */                         \  
               break;                                                \  
            }                                                        \  
            usleep(50);                                              \
       }                                                             \  
   } while (0)


#define OLED_FONT_SIZE   128*8

struct valid_t {
	uint8_t linux_valid;
	uint8_t rtos_valid;
} __attribute__((packed));

typedef union resv_t {
	struct valid_t valid;
	unsigned short mstime; // 0 : noblock, -1 : block infinite
} resv_t;

typedef struct cmdqu_t cmdqu_t;
struct cmdqu_t {
	uint8_t ip_id;
	uint8_t cmd_id : 7;
	uint8_t block : 1;
	union resv_t resv;
	unsigned int  param_ptr;
} __attribute__((packed)) __attribute__((aligned(0x8)));



enum SYSTEM_CMD_TYPE {
	CMDQU_SEND = 1,
	CMDQU_SEND_WAIT,
	CMDQU_SEND_WAKEUP,
};

#define RTOS_CMDQU_DEV_NAME "/dev/cvi-rtos-cmdqu"
#define RTOS_CMDQU_SEND                         _IOW('r', CMDQU_SEND, unsigned long)
#define RTOS_CMDQU_SEND_WAIT                    _IOW('r', CMDQU_SEND_WAIT, unsigned long)
#define RTOS_CMDQU_SEND_WAKEUP                  _IOW('r', CMDQU_SEND_WAKEUP, unsigned long)

enum SYS_CMD_ID {
    CMD_TEST_A  = 0x10,
    CMD_TEST_B,
    CMD_TEST_C,
    CMD_DUO_LED,
    SYS_CMD_INFO_LIMIT,
};

//Mailbox
#define OLED_SHOW
#define HAPPY   
#define PEACE
#define SAD
#define FEAR
#define ANGRY





#define IMAGE_WIDTH     1280
#define IMAGE_HEIGHT    720


uint8_t read_bit(uint8_t value,uint8_t position);
uint8_t count_bit(uint16_t data);


#endif


