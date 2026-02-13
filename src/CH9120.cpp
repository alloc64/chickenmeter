#include "CH9120.h"
#include <pinmode_arduino.h>

unsigned char tx[8] = {0x57, 0xAB};

/******************************************************************************
function:	Send four bytes
parameter:
    data: parameter
    command: command code
Info:  Set mode, enable port, clear serial port, switch DHCP, switch port 2
******************************************************************************/
void CH9120_TX_4_bytes(unsigned char data, int command) {
    for (int i = 2; i < 4; i++) {
        if (i == 2)
            tx[i] = command;
        else
            tx[i] = data;
    }
    DEV_Delay_ms(10);
    for (int o = 0; o < 4; o++)
        uart_putc(uart1, tx[o]);
    DEV_Delay_ms(10);
    for (int i = 2; i < 4; i++)
        tx[i] = 0;
}

/******************************************************************************
function:	Send five bytes
parameter:
    data: parameter
    command: command code
Info:  Set the local port and target port
******************************************************************************/
void CH9120_TX_5_bytes(UWORD data, int command) {
    unsigned char Port[2];
    Port[0] = data & 0xff;
    Port[1] = data >> 8;
    for (int i = 2; i < 5; i++) {
        if (i == 2)
            tx[i] = command;
        else
            tx[i] = Port[i - 3];
    }
    DEV_Delay_ms(10);
    for (int o = 0; o < 5; o++)
        uart_putc(uart1, tx[o]);
    DEV_Delay_ms(10);
    for (int i = 2; i < 5; i++)
        tx[i] = 0;
}

/******************************************************************************
function:	Send seven bytes
parameter:
    data: parameter
    command: command code
Info:  Set the IP address, subnet mask, gateway,
******************************************************************************/
void CH9120_TX_7_bytes(unsigned char data[], int command) {
    for (int i = 2; i < 7; i++) {
        if (i == 2)
            tx[i] = command;
        else
            tx[i] = data[i - 3];
    }
    DEV_Delay_ms(10);
    for (int o = 0; o < 7; o++)
        uart_putc(uart1, tx[o]);
    DEV_Delay_ms(10);
    for (int i = 2; i < 7; i++)
        tx[i] = 0;
}

void CH9120_TX_BAUD(UDOUBLE data, int command) {
    unsigned char Port[4];
    Port[0] = (data & 0xff);
    Port[1] = (data >> 8) & 0xff;
    Port[2] = (data >> 16) & 0xff;
    Port[3] = data >> 24;

    for (int i = 2; i < 7; i++) {
        if (i == 2)
            tx[i] = command;
        else
            tx[i] = Port[i - 3];
    }
    DEV_Delay_ms(10);
    for (int o = 0; o < 7; o++)
        uart_putc(uart1, tx[o]);
    DEV_Delay_ms(10);
    for (int i = 2; i < 7; i++)
        tx[i] = 0;
}

void CH9120_Start() {
    gpio_put(RES_PIN, 1);
    gpio_put(CFG_PIN, 0);
    DEV_Delay_ms(500);
}

void CH9120_End() {
    tx[2] = 0x0d;
    uart_puts(uart1, reinterpret_cast<const char *>(tx));
    DEV_Delay_ms(200);
    tx[2] = 0x0e;
    uart_puts(uart1, reinterpret_cast<const char *>(tx));
    DEV_Delay_ms(200);
    tx[2] = 0x5e;
    uart_puts(uart1, reinterpret_cast<const char *>(tx));
    DEV_Delay_ms(200);
    gpio_put(CFG_PIN, 1);
}

// TCP_SERVER、TCP_CLIENT、UDP_SERVER、UDP_CLIENT
void CH9120_SetMode(unsigned char Mode) {
    CH9120_TX_4_bytes(Mode, Mode1); //Mode
    DEV_Delay_ms(100);
}

void CH9120_SetLocalIP(unsigned char CH9120_LOCAL_IP[]) {
    CH9120_TX_7_bytes(CH9120_LOCAL_IP, LOCAL_IP); //LOCALIP
    DEV_Delay_ms(100);
}

void CH9120_SetSubnetMask(unsigned char CH9120_SUBNET_MASK[]) {
    CH9120_TX_7_bytes(CH9120_SUBNET_MASK, SUBNET_MASK); //SUBNET MASK
    DEV_Delay_ms(100);
}

void CH9120_SetGateway(unsigned char CH9120_GATEWAY[]) {
    CH9120_TX_7_bytes(CH9120_GATEWAY, GATEWAY); //GATEWAY
    DEV_Delay_ms(100);
}

void CH9120_SetTargetIP(unsigned char CH9120_TARGET_IP[]) {
    CH9120_TX_7_bytes(CH9120_TARGET_IP, TARGET_IP1); //TARGET IP
    DEV_Delay_ms(100);
}

void CH9120_SetLocalPort(UWORD CH9120_PORT) {
    CH9120_TX_5_bytes(CH9120_PORT, LOCAL_PORT1); //Local port
    DEV_Delay_ms(100);
}

void CH9120_SetTargetPort(UWORD CH9120_TARGET_PORT) {
    CH9120_TX_5_bytes(CH9120_TARGET_PORT, TARGET_PORT1); //Target port
    DEV_Delay_ms(100);
}

void CH9120_SetBaudRate(UDOUBLE CH9120_BAUD_RATE) {
    CH9120_TX_BAUD(CH9120_BAUD_RATE, UART1_BAUD1); //Port 1 baud rate
    DEV_Delay_ms(100);
}

void DEV_Delay_ms(UDOUBLE xms) {
    sleep_ms(xms);
}

void DEV_Delay_us(UDOUBLE xus) {
    sleep_us(xus);
}
