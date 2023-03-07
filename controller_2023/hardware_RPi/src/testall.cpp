static char VERSION[] = "XX.YY.ZZ";

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>


#include "clk.h"
#include "gpio.h"
#include "dma.h"
#include "pwm.h"
#include "version.h"

#include "ws2811.h"

#include <sys/ioctl.h>                                                                                                                                                                                          
#include <linux/i2c-dev.h>                                                                                                                                                                                      
                                                                                                                                                                                                                
#define ARRAY_SIZE(stuff)       (sizeof(stuff) / sizeof(stuff[0]))                                                                                                                                              
                                                                                                                                                                                                                
//PCA define                                                                                                                                                                                                    
#define NUMPCA 7                                                                                                                                                                                                
const int PCAaddr[] = {0x20, 0x1f, 0x22, 0x23, 0x5b, 0x5c, 0x5e};                                                                                                                                               
// const int PCAaddr[] = {0x20, 0x1f, 0x22, 0x5b, 0x5c};                                                                                                                                                        
// const int PCAaddr[] = {0x20, 0x5b, 0x5e};                                                                                                                                                                    
int I2C_init();                                                                                                                                                                                                 
int fd_PCA[NUMPCA] = {0};                                                                                                                                                                                       
// defaults for cmdline options                                                                                                                                                                                 
#define TARGET_FREQ             WS2811_TARGET_FREQ                                                                                                                                                              
#define GPIO_PIN                18                                                                                                                                                                              
#define DMA                     10                                                                                                                                                                              
#define STRIP_TYPE            WS2811_STRIP_GRB          // WS2812/SK6812RGB integrated chip+leds                                                                                                                
// #define STRIP_TYPE              WS2811_STRIP_GBR             // WS2812/SK6812RGB integrated chip+leds                                                                                                        
//#define STRIP_TYPE            SK6812_STRIP_RGBW               // SK6812RGBW (NOT SK6812RGB)                                                                                                                   
                                                                                                                                                                                                                
#define LIGHTNESS_MAX 100                                                                                                                                                                                       
int led_count[8] = {40,40,40,40,40,40,40,40};                                                                                                                                                                   
                                                                                                                                                                                                                
int clear_on_exit = 0;                                                                                                                                                                                          
                                                                                                                                                                                                                
int A0, A1, A2;                                                                                                                                                                                                 
                                                                                                                                                                                                                
ws2811_t ledstring =                                                                                                                                                                                            
{                                                                                                                                                                                                               
    .freq = TARGET_FREQ,                                                                                                                                                                                        
    .dmanum = DMA,                                                                                                                                                                                              
    .channel =                                                                                                                                                                                                  
    {                                                                                                                                                                                                           
        [0] =                                                                                                                                                                                                   
        {                                                                                                                                                                                                       
            .gpionum = GPIO_PIN,                                                                                                                                                                                
            .invert = 0,                                                                                                                                                                                        
            .count = 45,                                                                                                                                                                                        
            .strip_type = STRIP_TYPE,       
            .brightness = 255,
        }
    },
};

static uint8_t running = 1;
uint32_t color = 0x32;

void matrix_render(int channel)
{
    // ledstring.channel[0].leds[(y * width) + x] = matrix[channel][y * width + x];
    ledstring.channel[0].count = led_count[channel];
    for(int i=0; i<led_count[channel]; i++){
        ledstring.channel[0].leds[i] = color;
    }
}

void select_channel(int channel){
    switch(channel){
        case 0:
            if (write(A0, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio23/value");
            exit(11);
            }
            if (write(A1, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(12);
            }
            if (write(A2, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio25/value");
            exit(13);
            }
            break;
        case 1:
            if (write(A0, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio23/value");
            exit(14);
            }
            if (write(A1, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(15);
            }
            if (write(A2, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio25/value");
            exit(16);
            };
            break;
        case 2:
            if (write(A0, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio23/value");
            exit(17);
            }
            if (write(A1, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(18);
            }
            if (write(A2, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio25/value");
            exit(19);
            }
            break;
        case 3:
            if (write(A0, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio23/value");
            exit(20);
            }
            if (write(A1, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(21);
            }
            if (write(A2, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio25/value");
            exit(25);
            }
            break;
        case 4:
            if (write(A0, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio23/value");
            exit(26);
            }
            if (write(A1, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(27);
            }
            if (write(A2, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio25/value");
            exit(28);
            };
            break;
        case 5:
            if (write(A0, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio23/value");
            exit(29);
            }
            if (write(A1, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(30);
            }
            if (write(A2, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio25/value");
            exit(31);
            };
            break;
        case 6:
            if (write(A0, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio23/value");
            exit(32);
            }
            if (write(A1, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(33);
            }
            if (write(A2, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio25/value");
            exit(34);
            }
            break;
        case 7:
            if (write(A0, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio23/value");
            exit(35);
            }
            if (write(A1, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(36);
            }
            if (write(A2, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio25/value");
            exit(37);
            }
            break;
    }
}

static void ctrl_c_handler(int signum)
{
        (void)(signum);
    running = 0;
    unsigned char buffer[60] = {0};   
    color = 0;
    for(int i=0; i<8; i++){
                select_channel(i);
                matrix_render(i);
                ws2811_render(&ledstring);
                usleep(1000);
        }
        /*if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
        {
            fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
            break;
        }*/

        // 15 frames /sec
        buffer[0] = 0x88;

        for (int i = 0; i < 5; i++)
        {
                buffer[i*3 + 1] = (color & 0xFF0000) >> 16;
                buffer[i*3 + 2] = (color & 0x00FF00) >> 8 ;
                buffer[i*3 + 3] = (color & 0x0000FF)      ;
        }

        for (int i = 0; i < NUMPCA; i++)
        {
                if(write(fd_PCA[i], buffer, 16) != 16)
                {
                        printf("Failed to write to the I2C bus.\n");
                        return;
                }
        } 
    close(A0);
    close(A1);
    close(A2);

    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1)                                       
    {            
        perror("Unable to open /sys/class/gpio/unexpect");
        exit(1);                                          
    }           
     
    if (write(fd, "23", 2) != 2){
        perror("Error writing to /sys/class/gpio/unexpect");
        exit(1);                                            
    }           
     
    if (write(fd, "24", 2) != 2){
        perror("Error writing to /sys/class/gpio/unexpect");
        exit(1);                                            
    }           
     
    if (write(fd, "25", 2) != 2){
        perror("Error writing to /sys/class/gpio/unexpect");
        exit(1);                                            
    }           
     
}
 
static void setup_handlers(void)
{
    struct sigaction sa =
    {
        .sa_handler = ctrl_c_handler,
    };

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

void init_gpio(){

    int fd_export = open("/sys/class/gpio/export", O_WRONLY);
    
    if (fd_export == -1) {
        perror("Unable to open /sys/class/gpio/export");
        exit(1);
    }

    if (write(fd_export, "23", 2) != 2) {
        perror("Error writing to /sys/class/gpio/export");
        exit(1);
    }

    if (write(fd_export, "24", 2) != 2) {
        perror("Error writing to /sys/class/gpio/export");
        exit(1);
    }

    if (write(fd_export, "25", 2) != 2) {
        perror("Error writing to /sys/class/gpio/export");
        exit(1);
    }

    close(fd_export);
    // Set the pin to be an output by writing "out" to /sys/class/gpio/gpio24/direction

    int fd = open("/sys/class/gpio/gpio23/direction", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio23/direction");
        exit(1);
    }

    if (write(fd, "out", 3) != 3) {
        perror("Error writing to /sys/class/gpio/gpio23/direction");
        exit(1);
    }

    close(fd);

    // Set the pin to be an output by writing "out" to /sys/class/gpio/gpio24/direction

    fd = open("/sys/class/gpio/gpio24/direction", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio24/direction");
        exit(1);
    }

    if (write(fd, "out", 3) != 3) {
        perror("Error writing to /sys/class/gpio/gpio24/direction");
        exit(1);
    }

    close(fd);

    // Set the pin to be an output by writing "out" to /sys/class/gpio/gpio24/direction

    fd = open("/sys/class/gpio/gpio25/direction", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio25/direction");
        exit(1);
    }

    if (write(fd, "out", 3) != 3) {
        perror("Error writing to /sys/class/gpio/gpio25/direction");
        exit(1);
    }

    close(fd);


    A0 = open("/sys/class/gpio/gpio23/value", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio23/value");
        exit(1);
    }

    A1 = open("/sys/class/gpio/gpio24/value", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio24/value");
        exit(1);
    }

    A2 = open("/sys/class/gpio/gpio25/value", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio25/value");
        exit(1);
    }

}

void color_calc()
{
    static uint8_t rgb = 2;
    static uint8_t status = 1;      // 0 for dimming, 1 for lightning
    static uint8_t lightness = 1;

    if (lightness == 0)
    {
        if (rgb == 0)   rgb = 2;
        else rgb >>= 1;
        status = 1;
    }
    else if (lightness == LIGHTNESS_MAX)
    {
        status = 0;
    }

    if (status) lightness++;
    else        lightness--;

    color = lightness << (rgb * 8);
    return;
}

/*
void color_calc()
{
        if (color == 0x32)              color = 0x320000;
        else if (color == 0x320000)     color = 0x3200;
        else if (color == 0x3200)       color = 0x32;
}
*/

int main(int argc, char *argv[])
{

    init_gpio();
        // int fd_PCA[NUMPCA];
        int length;
        unsigned char buffer[60] = {0};

        // ----- OPEN THE I2C BUS -----
        for (int i = 0; i < NUMPCA; i++)
        {
                if ((fd_PCA[i] = I2C_init()) < 0)
                {
                        printf("I2C of %d init fail.\n", i);
                        return 1;
                }
                printf("File descriptor of %d opened at %d.\n", i, fd_PCA[i]);
                
                if (ioctl(fd_PCA[i], I2C_SLAVE, PCAaddr[i]) < 0)
                {
                        printf("Failed to acquire bus access and/or talk to slave %d", i);
                        return 2;
                }
        }
        printf("======================\n\n");

        // write to PCA
        buffer[0] = 0x45;
        buffer[1] = 0xFF;

        for (int i = 0; i < NUMPCA; i++)
        {
                if(write(fd_PCA[i], buffer, 2) != 2)
                {
                        printf("Failed to write to I2C bus.\n");
                        return 3;
                }
        }


    ws2811_return_t ret;

    sprintf(VERSION, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);

    setup_handlers();

    if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr,"ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
        return ret;
    }

    while (running)
    {
        color_calc();
        for(int i=0; i<8; i++){
                select_channel(i);
                matrix_render(i);
                ws2811_render(&ledstring);
                usleep(1500);
        }
        /*if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
        {
            fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
            break;
        }*/

        // 15 frames /sec
        buffer[0] = 0x88;

        for (int i = 0; i < 5; i++)
        {
                buffer[i*3 + 1] = (color & 0xFF0000) >> 16;
                buffer[i*3 + 2] = (color & 0x00FF00) >> 8 ;
                buffer[i*3 + 3] = (color & 0x0000FF)      ;
        }

        for (int i = 0; i < NUMPCA; i++)
        {
                if(write(fd_PCA[i], buffer, 16) != 16)
                {
                        printf("Failed to write to the I2C bus.\n");
                        return 4;
                }
        }
    }

    if (clear_on_exit) {
        matrix_render(0);
        ws2811_render(&ledstring);
    }

    ws2811_fini(&ledstring);
    printf ("\n");
    return ret;
}                                  

int I2C_init()
{
        int file_i2c;
        char *filename = (char*)"/dev/i2c-1";
        if ((file_i2c = open(filename, O_RDWR)) < 0)
        {
                printf("Failed to open i2c bus. Exit.\n");
                return -1;
        }
        return file_i2c;
}

