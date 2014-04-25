/*
 * pingpong.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/var_io.h>
#include <cyg/hal/var_io_gpio.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_if.h>
#include <cyg/io/io.h>
#include <cyg/kernel/kapi.h>

#include <cyg/mcc/mcc_api.h>

#define MCC_VERSION_STRING "001.002"

#define MCC_NODE_A5        (0)
#define MCC_NODE_M4        (0)
#define MCC_SENDER_PORT    (1)
#define MCC_RESPONDER_PORT (2)

static unsigned char stack_thread_consumer[CYGNUM_HAL_STACK_SIZE_TYPICAL];
static unsigned char stack_thread_tick[CYGNUM_HAL_STACK_SIZE_TYPICAL];

typedef struct the_message
{
    cyg_uint32  DATA;
} THE_MESSAGE;

cyg_handle_t thread_consumer;
cyg_handle_t thread_tick;

cyg_thread thread_consumer_data;
cyg_thread thread_tick_data;

unsigned int mqx_endpoint_a5[] = {0,MCC_NODE_A5,MCC_SENDER_PORT};
unsigned int mqx_endpoint_m4[] = {1,MCC_NODE_M4,MCC_RESPONDER_PORT};

void thread_consumer_fn(void)
{
    THE_MESSAGE     msg;
    unsigned int    num_of_received_bytes;
    char            mcc_info[255];
    int             ret_value;

    msg.DATA = 1;
    ret_value = mcc_initialize(MCC_NODE_M4);
    ret_value = mcc_get_info(MCC_NODE_M4, mcc_info);
    if(0 == ret_value && (strcmp(mcc_info, MCC_VERSION_STRING) != 0)) {
        diag_printf("Error, attempting to use different versions of the MCC library on each core! Application is stopped now.\n");
        while(1);
    }

    ret_value = mcc_create_endpoint(mqx_endpoint_m4, MCC_RESPONDER_PORT);

    diag_printf("Responder task started, MCC version is %s\n", mcc_info);
    while (1)
    {
        ret_value = mcc_recv_copy(mqx_endpoint_m4, &msg, sizeof(THE_MESSAGE), &num_of_received_bytes, 0xffffffff);
        if(0 != ret_value) {
            diag_printf("Responder task receive error: 0x%02x\n", ret_value);
        } else {
            diag_printf("Received message: Size = %u, DATA = 0x%02x\n", num_of_received_bytes, msg.DATA);
            msg.DATA++;
            ret_value = mcc_send(mqx_endpoint_a5, &msg, sizeof(THE_MESSAGE), 0xffffffff);
            diag_printf("mcc_send (msg = 0x%x) result = 0x%x\n", msg.DATA, ret_value);
        }
        cyg_thread_delay(100);
    }
}

void thread_tick_fn(void)
{
    while(1)
    {
        diag_printf("Tick.\n");
        cyg_thread_delay(5000);
    }
}

int main(int argc, char **argv)
{
    diag_printf("Welcome to the Colibri VF61 ping-pong test app.\n");

    cyg_thread_create(10, (cyg_thread_entry_t*) thread_consumer_fn, 0, "thread_consumer", &stack_thread_consumer[0], CYGNUM_HAL_STACK_SIZE_TYPICAL, &thread_consumer, &thread_consumer_data);
    cyg_thread_create(10, (cyg_thread_entry_t*) thread_tick_fn, 0, "thread_tick", &stack_thread_tick[0], CYGNUM_HAL_STACK_SIZE_TYPICAL, &thread_tick, &thread_tick_data);

    cyg_thread_resume(thread_consumer);
    cyg_thread_resume(thread_tick);

    while (1);

    return 0;
}
