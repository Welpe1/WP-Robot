#ifndef __ROBOT_UTILS_H
#define __ROBOT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include "cvi_tdl.h"

#define __DEBUG__
#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif


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


#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
#define LIMIT_MAX(x, max) ((x) > (max) ? (max) : (x))
#define LIMIT_MIN(x, min) ((x) < (min) ? (min) : (x))



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


#define IMAGE_WIDTH     1280
#define IMAGE_HEIGHT    720


uint8_t face_filter(cvtdl_face_t *stFaceMeta);
uint8_t wp_read_bit(uint64_t value,uint8_t position);
void wp_set_bit(uint64_t *value,uint8_t position,uint8_t bit);
uint8_t wp_count_bit(uint64_t value);
uint16_t rand_section(uint16_t start,uint16_t end);
void print_binary(uint16_t n);
void file_search_data(FILE *fp,char *target,long int *buf);
int extract_first_int(const char *str);
double cal_time_elapsed(struct timeval start, struct timeval end);

#endif


