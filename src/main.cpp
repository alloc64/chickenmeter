#include "Adafruit_SHT31.h"
#include "CH9120.h"

#define IP_SIZE 4
#define BUFFER_LENGTH 1024
unsigned char buffer[BUFFER_LENGTH];
unsigned int payloadLength = 0;

MbedI2C wireI2c0(2, 3);
Adafruit_SHT31 sensorI2c0(&wireI2c0);

MbedI2C wireI2c1(4, 5);
Adafruit_SHT31 sensorI2c1(&wireI2c1);

void setupSensor(Adafruit_SHT31 &sensor) {
    if (!sensor.begin(0x44))
        Serial.println("Sensor1 not found!");
}

void readSensor(Adafruit_SHT31 &sensor, float &t, float &h) {
    t = 0.0f;
    h = 0.0f;
    sensor.readBoth(&t, &h);
}

void initEthernet() {
    uart_init(uart1, Inti_BAUD_RATE);
    gpio_set_function(UART_TX_PIN1, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN1, GPIO_FUNC_UART);

    pinMode(CFG_PIN, OUTPUT);
    pinMode(RES_PIN, OUTPUT);

    memset(buffer, 0, BUFFER_LENGTH);

    unsigned char CH9120_LOCAL_IP[IP_SIZE] = {192, 168, 31, 249};
    unsigned char CH9120_GATEWAY[IP_SIZE] = {192, 168, 31, 1};
    unsigned char CH9120_SUBNET_MASK[IP_SIZE] = {255, 255, 255, 0};
    uint16_t CH9120_PORT = 1000;

    CH9120_Start();
    CH9120_SetMode(TCP_SERVER);
    CH9120_SetLocalIP(CH9120_LOCAL_IP);
    CH9120_SetSubnetMask(CH9120_SUBNET_MASK);
    CH9120_SetGateway(CH9120_GATEWAY);
    CH9120_SetLocalPort(CH9120_PORT);
    CH9120_SetBaudRate(115200);
    CH9120_End();

    uart_set_baudrate(uart1, Transport_BAUD_RATE);
    while (uart_is_readable(uart1)) {
        uart_getc(uart1);
    }

    Serial.print("Listening at: ");
    for (int i = 0; i < IP_SIZE; i++) {
        Serial.print(CH9120_LOCAL_IP[i]);

        if (i < IP_SIZE -1)
            Serial.print(".");
    }
    Serial.print(":");
    Serial.println(CH9120_PORT);
}

void sendHttpResponse(const char *body) {
    char response[256];

    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %d\r\n"
             "\r\n"
             "%s",
             strlen(body),
             body);

    uart_puts(uart1, response);
}

void dispatchEthernet() {
    while (uart_is_readable(uart1)) {
        unsigned char ch1 = uart_getc(uart1);

        // drop large payloads
        if (payloadLength >= BUFFER_LENGTH - 1)
            continue;

        buffer[payloadLength++] = ch1;
        buffer[payloadLength] = 0;
    }

    bool isComplete = payloadLength >= 4 &&
                  buffer[payloadLength-4] == '\r' &&
                  buffer[payloadLength-3] == '\n' &&
                  buffer[payloadLength-2] == '\r' &&
                  buffer[payloadLength-1] == '\n';

    if (!isComplete)
        return;

    payloadLength = 0;

    float t0, h0, t1, h1;
    readSensor(sensorI2c0, t0, h0);
    readSensor(sensorI2c1, t1, h1);

    char body[256];
    snprintf(body, sizeof(body),
             "<html><body>"
             "<h1>Senzor 1</h1>"
             "<p>Temperature: %.2f C</p>"
             "<p>Humidity: %.2f %%</p>"
             "<h1>Senzor 2</h1>"
             "<p>Temperature: %.2f C</p>"
             "<p>Humidity: %.2f %%</p>"
             "</body></html>",
             t0, h0, t1, h1);

    sendHttpResponse(body);

}


void setup() {
    Serial.begin(9600);
    delay(2000);

    initEthernet();

    setupSensor(sensorI2c0);
    setupSensor(sensorI2c1);
}
void loop() {
    dispatchEthernet();
}
