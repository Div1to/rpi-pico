#include "hardware/gpio.h"
#include "pico/stdlib.h"

/******
 * gpio soft i2c core data type
*******/

enum gpio_i2c_baudrate{
    GPIO_I2C_USPB_MAX      = 0,
    GPIO_I2C_USPB_FAST     = 1,
    GPIO_I2C_USPB_STANDARD = 5
};

typedef struct {
    uint sda;
    uint scl;
    bool master;
    uint tu;   //change data allow and data valid time
    uint8_t addr;
} gpio_i2c;

/******
 * gpio software i2c public metadata accessor
*******/

/**
 * \brief initialize a gpio software i2c bus object
 * \param bus empty gpio_i2c object
 * \param sda sda pin for bus
 * \param scl scl pin for bus
 * \param master 1 for master 0 for slave
*/
void gpio_i2c_init(gpio_i2c* bus,uint sda,uint scl,bool master);
/**
 * \brief setup bus running in master mode or not
 * \param bus gpio_i2c object
 * \param master 1 for master 0 for slave
*/
void gpio_i2c_master(gpio_i2c* bus,bool master);
/**
 * \brief set the minimal time cost for 1bit
 * \param bus gpio_i2c object
 * \param baudrate GPIO_I2C_USPB_STANDARD or GPIO_I2C_USPB_FAST or GPIO_I2C_USPB_MAX
*/
void gpio_i2c_set_baudrate(gpio_i2c* bus,enum gpio_i2c_baudrate baudrate);
/**
 * \brief set the address for gpio_i2c slave object
 * \param bus gpio_i2c object
 * \param address gpio_i2c slave address
*/
void gpio_i2c_set_addr(gpio_i2c* bus,uint8_t address);
/**
 * \brief deinitialize a gpio software i2c bus object
 * \param bus empty gpio_i2c object
*/
void gpio_i2c_deinit(gpio_i2c* bus);

/******
 * gpio software i2c endless API
*******/

/**
 * \brief master write any bytes to slave
 * \param bus gpio_i2c object
 * \param address receiver's address
 * \param us  wake up time out
 * \return 0 for ack 1 for nack
 * \note some device will not ack but it's actually waked up
 * \note na=gpio_i2c_wake(bus,address,800);
*/
bool gpio_i2c_wake(gpio_i2c* bus,uint8_t address,int us);

/**
 * \brief master write any bytes to slave
 * \param bus gpio_i2c object
 * \param address receiver's address
 * \param bytes data will be sent
 * \param size length of bytes
*/
bool gpio_i2c_write(gpio_i2c* bus,uint8_t address,uint8_t* bytes,int size);
/**
 * \brief master read any bytes from slave
 * \param bus gpio_i2c object
 * \param address transmiter's address
 * \param bytes data from transmiter
 * \param size length of data will be received
 * \param us delay some us before read
 * \return pointer to result
 * \note
 * some device need to wait some seconds after read command was sent
*/
int gpio_i2c_read(gpio_i2c* bus,uint8_t address,uint8_t* rbytes,int size,int us);
/**
 * \brief slave reply any data to master
 * \param bus gpio_i2c object
 * \param bytes data will be sent
*/
uint gpio_i2c_reply(gpio_i2c* bus,uint8_t* bytes);