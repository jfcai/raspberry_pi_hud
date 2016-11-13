#ifndef __OLED_H
#define __OLED_H           
#include <stdlib.h>  
#include <bcm2835.h>     

#define OLED_MODE 		0
#define SIZE 			16
#define XLevelL   		0x02
#define XLevelH   		0x10
#define Max_Column  	0x3F      // 256/4-1
#define Max_Row   		0x3F      // 64-1
#define Brightness  	0xFF
#define Shift   		0x1C

/*-----------------OLED端口定义 4线SPI---------------- */ 					   
/* 采用IO口模拟SPI通讯方式驱动oled模块  */
#define OLED_DC			RPI_V2_GPIO_P1_16	// Data/Command Control
#define OLED_RST		RPI_V2_GPIO_P1_18	// Power Reset
#define OLED_SCLK		RPI_V2_GPIO_P1_15	// Serial Clock of SPI
#define OLED_SDIN		RPI_V2_GPIO_P1_13	// Serial Data of SPI


#define OLED_RST_Clr() 		bcm2835_gpio_clr(OLED_RST)//RES
#define OLED_RST_Set() 		bcm2835_gpio_set(OLED_RST)

#define OLED_DC_Clr() 		bcm2835_gpio_clr(OLED_DC)//DC
#define OLED_DC_Set() 		bcm2835_gpio_set(OLED_DC)

#define OLED_SCLK_Clr() 	bcm2835_gpio_clr(OLED_SCLK)//CLK
#define OLED_SCLK_Set() 	bcm2835_gpio_set(OLED_SCLK)

#define OLED_SDIN_Clr() 	bcm2835_gpio_clr(OLED_SDIN)//DIN
#define OLED_SDIN_Set() 	bcm2835_gpio_set(OLED_SDIN)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据





void OLED_WR_Byte(unsigned char dat,unsigned char cmd);
void OLED_Init(void);
void Set_Column_Address(unsigned char a, unsigned char b);
void Set_Row_Address(unsigned char a, unsigned char b);
void Set_Write_RAM(void);
void Set_Read_RAM(void);
void Fill_RAM(unsigned char Data);
void Draw_Rectangle(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e);
void Fill_Block(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void Grayscale(void);
void Set_Remap_Format(unsigned char d);
void Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void Con_4_byte(unsigned char DATA);
void Set_Partial_Display(unsigned char a, unsigned char b, unsigned char c);
void Set_Display_Offset(unsigned char d);
void Set_Start_Line(unsigned char d);
void Set_Master_Current(unsigned char d);
void Vertical_Scroll(unsigned char a, unsigned char b, unsigned char c);
void Set_Gray_Scale_Table(void);
void Set_Linear_Gray_Scale_Table(void); 
void Display_Str(unsigned char x,unsigned char y,unsigned char *str,unsigned char font_size);
void Display_Number(unsigned char x,unsigned char y,const unsigned char *str);
void Fill_RAM_DEALY(unsigned char Data,unsigned long s);

#endif