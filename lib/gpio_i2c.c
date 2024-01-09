#include "gpio_i2c.h"


/******
 * gpio software i2c public metadata accessor
*******/

void gpio_i2c_init(gpio_i2c* bus,uint sda,uint scl,bool master){
    bus->sda = sda;
    bus->scl = scl;
    gpio_i2c_master(bus,true);
    gpio_i2c_set_baudrate(bus,GPIO_I2C_USPB_STANDARD);
}
void gpio_i2c_master(gpio_i2c* bus,bool master){
    if(master){
        gpio_set_dir(bus->sda, GPIO_OUT);
        gpio_set_dir(bus->scl, GPIO_OUT);
    }else{
        gpio_set_dir(bus->sda, GPIO_IN);
        gpio_set_dir(bus->scl, GPIO_IN);
    }
    gpio_put(bus->sda, 1);
    gpio_put(bus->scl, 1);
    gpio_set_function(bus->sda, GPIO_FUNC_SIO);
    gpio_set_function(bus->scl, GPIO_FUNC_SIO);
}
void gpio_i2c_set_baudrate(gpio_i2c* bus,enum gpio_i2c_baudrate baudrate){
    bus->tu=baudrate;
}
void gpio_i2c_set_addr(gpio_i2c* bus,uint8_t address){
    bus->addr=address;
}
void gpio_i2c_deinit(gpio_i2c* bus){
    gpio_set_function(bus->sda, GPIO_FUNC_NULL);
    gpio_set_function(bus->scl, GPIO_FUNC_NULL);
}

/******
 * gpio software i2c internel function
*******/
#define GPIO_I2C_R 1
#define GPIO_I2C_W 0
#define GPIO_I2C_W_S 0
#define GPIO_I2C_W_P 1
#define GPIO_I2C_RW_A 0
#define GPIO_I2C_RW_NA 1
/**
 * \brief     write a condition
 * \param bus gpio software i2c bus
 * \param condition 0 for START 1 for STOP
 * \note gpio_i2c_rw_c(bus,GPIO_I2C_W_S)
 * \note gpio_i2c_rw_c(bus,GPIO_I2C_W_P)
*/
static void gpio_i2c_w_c(gpio_i2c* bus,bool condition){
    // for initial and stop scl is height   
    // if not the communication should some issue
    // sda ''''.....START
    // scl ''''''''.
    // sda ....'''''STOP
    // scl .''''''''
    
    gpio_set_dir(bus->sda,GPIO_OUT);
    // STOP Condition
    if(condition) gpio_put(bus->scl,condition);
    // common STOP/START process
    sleep_us(bus->tu);
    gpio_put(bus->sda,condition);
    sleep_us(bus->tu);
    gpio_put(bus->scl,condition);
}

/**
 * \brief     read or write a bit
 * \param bus gpio software i2c bus
 * \param rw  0 for write 1 for read
 * \param nab nack, ack or single bit
 * \note gpio_i2c_rw_1(bus,GPIO_I2C_W,bit)
 * \note gpio_i2c_rw_1(bus,GPIO_I2C_R,bit)
*/
static bool gpio_i2c_rw_1(gpio_i2c* bus,bool rw,bool nab){
    // sda ''''????? READ
    // scl ....''''.
    // sda ????bbbbb WRITE
    // scl ....''''.

    // change data allowed stage
    sleep_us(bus->tu);

    // if rw=1(read) sda will automaticly pulled up
    // if rw=0(write) set the sda at CDA stage
    if(!rw) gpio_put(bus->sda,nab);

    // raise scl to DV stage
    gpio_put(bus->scl,true);
    sleep_us(bus->tu);

    // always return the logic read from bus
    nab=gpio_get(bus->sda);
    gpio_put(bus->scl,false);
    return nab;
}

/**
 * \brief     read or write a byte
 * \param bus gpio software i2c bus
 * \param rw  0 for write 1 for read
 * \param nab nack, ack or single byte
 * \note gpio_i2c_rw_1(bus,GPIO_I2C_W,bit)
 * \note gpio_i2c_rw_1(bus,GPIO_I2C_R,bit)
*/
static uint8_t gpio_i2c_rw_8(gpio_i2c* bus,bool rw,uint8_t nab){
    uint8_t mask=0b10000000;
    uint8_t r_byte=0x0;

    // read and write stage
    // if rw=0(write) dir=1(OUT)
    // if rw=1(read) dir=0(IN)
    gpio_set_dir(bus->sda,!rw);
    for(int i=0;i<8;i++){
        // take per bit from nab(byte)
        bool bit=nab & mask;
        // always record the bit from rw_1
        if(gpio_i2c_rw_1(bus,rw,bit)) r_byte = r_byte | mask;
        mask=mask >> 1;
    }

    // ack stage
    // if rw=0(write) dir(ack)=0(IN) dir(rw_1)=1
    // if rw=1(read)  dir(ack)=1(OUT) dir(rw_1)=0
    gpio_set_dir(bus->sda,rw);
    nab=gpio_i2c_rw_1(bus,!rw,1 & nab);

    // return stage
    // if rw=0(write) nab=n/ack(return) r_bytes=nouse
    // if rw=1(read)  nab=n/ack(send)   rbytes=return
    if(!rw) r_byte=nab;
    
    return r_byte;
}

bool gpio_i2c_wake(gpio_i2c* bus,uint8_t address,int us){
    gpio_i2c_w_c(bus,GPIO_I2C_W_S);
    bool na=gpio_i2c_rw_8(bus,GPIO_I2C_W,address<<1);
    sleep_us(us);
    gpio_i2c_w_c(bus,GPIO_I2C_W_P);
    return na;
}

bool gpio_i2c_write(gpio_i2c* bus,uint8_t address,uint8_t* bytes,int size){
    gpio_i2c_w_c(bus,GPIO_I2C_W_S);
    int na=GPIO_I2C_RW_NA;
    int len=size+1;
    uint8_t frame[len];
    frame[0]=address<<1;
    for(int i=0;i<size;i++) frame[i+1]=bytes[i];
    for(int i=0;i<len;i++){
        na=gpio_i2c_rw_8(bus,GPIO_I2C_W,frame[i]);
        if(na) break;
    }
    gpio_i2c_w_c(bus,GPIO_I2C_W_P);
    return !na;
}

int gpio_i2c_read(gpio_i2c* bus,uint8_t address,uint8_t* rbytes,int size,int us){
    gpio_i2c_w_c(bus,GPIO_I2C_W_S);
    int na=GPIO_I2C_RW_NA;
    address=(address<<1) | 0b00000001;
    // stage 1 send read command
    na=gpio_i2c_rw_8(bus,GPIO_I2C_W,address);
    // stage 2 read data
    if(!na){
        sleep_us(us);
        for(int i=0;i<size;i++)
            rbytes[i]=gpio_i2c_rw_8(bus,GPIO_I2C_R,i>(size-1));// send nack if last byte
    }
    gpio_i2c_w_c(bus,GPIO_I2C_W_P);
    return 1;
}