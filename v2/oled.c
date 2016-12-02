#include "oled.h"
#include <iconv.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <sys/stat.h>
#include <unistd.h>
#include "Number32X64.h"



#define DOTS_BYTES(font) (FONT_WIDTH[font] * FONT_HEIGHT[font] / 8)
#define HZ_INDEX(hz)    ((hz[0] - 0xa1) * 94 + (hz[1] - 0xa1))




/**
 * 转换字符编码
 * @from_charset
 * @to_charset
 * @inbuf
 * @inlen
 * @return int
 */
int code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen,  
        char *outbuf, size_t outlen) {  
    iconv_t cd;  
    char **pin = &inbuf;  
    char **pout = &outbuf;  
  
    cd = iconv_open(to_charset, from_charset);  
    if (cd == 0)  
        return -1;  
    memset(outbuf, 0, outlen);  
    if (iconv(cd, pin, &inlen, pout, &outlen) == -1)  
        return -1;  
    iconv_close(cd);  
    *pout = 0;  
  
    return 0;  
}  
  
/** 转换utf-8 到 gb2312
 * @param
 * @param
 * @param
 * @param
 * @return
 */
int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {  
    return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);  
}  
  

/** 转换gb2312 到 utf-8 */
int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {  
    return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);  
} 


void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{ 
  unsigned char i;   
  if(cmd)
    OLED_DC_Set();
  else 
    OLED_DC_Clr();   
  nanosleep(10);
  for(i=0;i<8;i++)
  {       
    OLED_SCLK_Clr();
    if(dat&0x80)
      OLED_SDIN_Set();
    else 
      OLED_SDIN_Clr();
       
    //等待数据稳定
    //拉高时钟，让设备接收数据
    nanosleep(1);
    OLED_SCLK_Set();

    //等待设备接收数据
    nanosleep(1);
    dat<<=1;   
  }
 //OLED_SCLK_Set();
 //OLED_DC_Set();  
}



void Set_Column_Address(unsigned char a, unsigned char b)
{
    OLED_WR_Byte(0x15,OLED_CMD);            // Set Column Address
    OLED_WR_Byte(a,OLED_DATA);          //   Default => 0x00
    OLED_WR_Byte(b,OLED_DATA);          //   Default => 0x77
}


void Set_Row_Address(unsigned char a, unsigned char b)
{
    OLED_WR_Byte(0x75,OLED_CMD);            // Set Row Address
    OLED_WR_Byte(a,OLED_DATA);          //   Default => 0x00
    OLED_WR_Byte(b,OLED_DATA);          //   Default => 0x7F
}


void Set_Write_RAM()
{
    OLED_WR_Byte(0x5C,OLED_CMD);            // Enable MCU to Write into RAM
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
 //清屏函数,清屏后,整个屏幕是黑的
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM(unsigned char Data)
{
unsigned char i,j;
    Set_Column_Address(0x00,0x77);
    Set_Row_Address(0x00,0x7f);
    Set_Write_RAM();
    for(i=0;i<128;i++)
    {
        for(j=0;j<120;j++)
        {
            OLED_WR_Byte(Data,OLED_DATA);
            OLED_WR_Byte(Data,OLED_DATA); 
        }
    }
}



unsigned char GetByte(unsigned char d){
  switch(d){
    case 0x00:
      return 0x00;
    case 0x40:
      return 0x0f;
    case 0x80:
      return 0xf0;
    case 0xc0:
      return 0xff;
    default:
      return 0x00;
  }
}

void Con_4_byte(unsigned char DATA)
{
  int i;
  unsigned char d;
  d=DATA;
 
  for(i=0;i<4;i++){
    OLED_WR_Byte(GetByte(d&0xc0),OLED_DATA);
    d = d << 2;
  }
}





void OLED_INIT(void)
{

    OLED_RST_Set();
    bcm2835_delay(500);
    OLED_RST_Clr();
    bcm2835_delay(500);
    OLED_RST_Set();

    
    OLED_WR_Byte(0xFD,OLED_CMD); // Set Command Lock
    OLED_WR_Byte(0x12,OLED_DATA); //
    
    OLED_WR_Byte(0xB3,OLED_CMD); // Set Clock as 80 Frames/Sec  原为0x91
    OLED_WR_Byte(0xD0,OLED_DATA);

    OLED_WR_Byte(0xCA,OLED_CMD); // Set Multiplex Ratio
    OLED_WR_Byte(0x3F,OLED_DATA); // 1/64 Duty (0x0F~0x5F)
   
    OLED_WR_Byte(0xA2,OLED_CMD); // Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_DATA); //    
    
    OLED_WR_Byte(0xA1,OLED_CMD); // Set Mapping RAM Display Start Line (0x00~0x7F)
    OLED_WR_Byte(0x00,OLED_DATA); //    

    
    OLED_WR_Byte(0xA0,OLED_CMD);  //Set Column Address 0 Mapped to SEG0 
    OLED_WR_Byte(0x14,OLED_DATA); //   Default => 0x40
                                  //     Horizontal Address Increment
                                  //     Column Address 0 Mapped to SEG0
                                  //     Disable Nibble Remap
                                  //     Scan from COM0 to COM[N-1]
                                  //     Disable COM Split Odd Even
    OLED_WR_Byte(0x11,OLED_DATA); //    Default => 0x01 (Disable Dual COM Mode)

    
    OLED_WR_Byte(0xB5,OLED_CMD);  //  Disable GPIO Pins Input
    OLED_WR_Byte(0x00,OLED_DATA); //    
    
    OLED_WR_Byte(0xAB,OLED_CMD);  //   Enable Internal VDD Regulator
    OLED_WR_Byte(0x01,OLED_DATA); //

    OLED_WR_Byte(0xB4,OLED_CMD);  //  Display Enhancement  
    OLED_WR_Byte(0xA0,OLED_DATA); // Enable External VSL
    OLED_WR_Byte(0xF8,OLED_DATA); // Enhance Low Gray Scale Display Quality

    OLED_WR_Byte(0xC1,OLED_CMD);  //  Set Contrast Current 
    OLED_WR_Byte(0x7F,OLED_DATA); //  Default => 0x7F

    OLED_WR_Byte(0xC7,OLED_CMD);  //  Master Contrast Current Control 
    OLED_WR_Byte(Brightness,OLED_DATA); //  Default => 0x0f (Maximum)

 //   OLED_WR_Byte(0xB9,OLED_CMD);
    OLED_WR_Byte(0xB8,OLED_CMD); //     // Set Gray Scale Table 
    OLED_WR_Byte(0x0C,OLED_DATA); //
    OLED_WR_Byte(0x18,OLED_DATA); //
    OLED_WR_Byte(0x24,OLED_DATA); //
    OLED_WR_Byte(0x30,OLED_DATA); //
    OLED_WR_Byte(0x3C,OLED_DATA); //
    OLED_WR_Byte(0x48,OLED_DATA); //
    OLED_WR_Byte(0x54,OLED_DATA); //
    OLED_WR_Byte(0x60,OLED_DATA); //
    OLED_WR_Byte(0x6C,OLED_DATA); //
    OLED_WR_Byte(0x78,OLED_DATA); //
    OLED_WR_Byte(0x84,OLED_DATA); //
    OLED_WR_Byte(0x90,OLED_DATA); //
    OLED_WR_Byte(0x9C,OLED_DATA); //
    OLED_WR_Byte(0xA8,OLED_DATA); //
    OLED_WR_Byte(0xB4,OLED_DATA); //
    OLED_WR_Byte(0x00,OLED_CMD);    // Enable Gray Scale Table

    
    OLED_WR_Byte(0xB1,OLED_CMD); //   Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
    OLED_WR_Byte(0xE2,OLED_DATA); //  Default => 0x74 (7 Display Clocks [Phase 2] / 9 Display Clocks [Phase 1])

    OLED_WR_Byte(0xD1,OLED_CMD); //    Enhance Driving Scheme Capability (0x00/0x20)
    OLED_WR_Byte(0xA2,OLED_DATA); //   Normal
    OLED_WR_Byte(0x20,OLED_DATA); //

   
    OLED_WR_Byte(0xBB,OLED_CMD); //     Set Pre-Charge Voltage Level as 0.60*VCC
    OLED_WR_Byte(0x1F,OLED_DATA); //

    OLED_WR_Byte(0xB6,OLED_CMD); //  // Set Second Pre-Charge Period as 8 Clocks 
    OLED_WR_Byte(0x08,OLED_DATA); //

    OLED_WR_Byte(0xBE,OLED_CMD); //  Set Common Pins Deselect Voltage Level as 0.86*VCC 
    OLED_WR_Byte(0x07,OLED_DATA); //  Default => 0x04 (0.80*VCC)

    OLED_WR_Byte(0xA6,OLED_CMD); //     Normal Display Mode (0x00/0x01/0x02/0x03)
                                      //   Default => 0xA4
                        //     0xA4  => Entire Display Off, All Pixels Turn Off
                        //     0xA5  => Entire Display On, All Pixels Turn On at GS Level 15
                        //     0xA6  => Normal Display
                        //     0xA7  => Inverse Display
     
     OLED_WR_Byte(0xA9,OLED_CMD); //   Disable Partial Display
        
   
   
   OLED_WR_Byte(0xAF,OLED_CMD); // Display On

   bcm2835_delay(100);

    
 
}



/***************************************************************
//  显示16*16点阵汉字 2015-05晶奥测试通过
//  取模方式为：横向取模左高位,数据排列:从左到右从上到下    16列 16行 
//   num：汉字在字库中的位置
//   x: Start Column  开始列 范围 0~（256-16）
//   y: Start Row   开始行 0~63 
***************************************************************/
void HZ24_24( unsigned char x, unsigned char y, char *str)
{
  unsigned char x1,i ;
  unsigned char dots[72];
  char buf[2] = {0};
  FILE* hzk;

  x1=x/4; 
  Set_Column_Address(Shift+x1,Shift+x1+5); // 设置列坐标，shift为列偏移量由1322决定。3为24/4-1
  Set_Row_Address(y,y+23); 
  Set_Write_RAM();   // 写显存
  
  /* 打开汉字库hzk16，并从中提取“宋”字的点阵数据 */
    if((hzk = fopen("/home/pi/hzk24h", "rb")) == NULL)
    {
      printf("%s\n","没有文件");
        return ;
    }

    u2g(str,strlen(str),buf,2);

    fseek(hzk, HZ_INDEX(buf) * 72, SEEK_SET);
    fread(dots, sizeof(unsigned char), 72, hzk);
    fclose(hzk);


  for (i=23;i>0;i--)
    {
        Con_4_byte(dots[i*3+0]);
        Con_4_byte(dots[i*3+1]);
        Con_4_byte(dots[i*3+2]);
    }
    
    // for (i=0;i<72;i++)
    // {
    //   Con_4_byte(dots[i]);
    //   printf("%02x,",dots[i]);
    // }   
}




//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Devided by 4)
//    c: Row Address of Start
//    d: Row Address of End
// 灰度模式下显示一副图片
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Pattern(unsigned char ch, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
       
  int i,j;
 
  //取模时候像素正序  （不能反序与2.7不同）
  if(ch == ' '){
    j = 10 * 1024;
  }
  else{
    j = (ch - 0x30) * 1024;
  }
  Set_Column_Address(Shift+a,Shift+b);
  Set_Row_Address(c,d);
  Set_Write_RAM();

  for(i = 0;i < 1024;i++)
  {
    OLED_WR_Byte(gImage[j+i],OLED_DATA);
  }
  // for(i=0;i<(d-c+1);i++)
  // {
  //   for(j=0;j<(b-a+1);j++)
  //   {
  //     OLED_WR_Byte(*Src_Pointer,OLED_DATA);
  //     Src_Pointer++;
  //     OLED_WR_Byte(*Src_Pointer,OLED_DATA);
  //     Src_Pointer++;
  //   }
  // }

 }




/**
 * 显示一个中文
 * @x: 开始列 范围 0 ~ (256 - font_size)
 * @y: 开始行 0 ~ 63
 * @str: 要显示的字符
 * @font_size: 16 或 24
 */
void Display_1_Chinese(unsigned char x,unsigned char y,char *str,unsigned char font_size)
{
  unsigned char x1,i;
  unsigned char dots[72] = {0};
  char fontPath[100] = {0};
  int dotsLen = 0;

  dotsLen = font_size * font_size / 8;

  FILE *fp;
  x1 = x / 4;


  sprintf(fontPath,"/home/pi/fonts/宋体%d.dot",font_size);

  //打开字库文件
  if((fp = fopen(fontPath,"rb")) == NULL)
  {
    printf("字体文件 %s 打开失败!\n",fontPath);
    return;
  }

  //从点陈字库中读取点陈码
  fseek(fp,HZ_INDEX(str) * dotsLen,SEEK_SET);
  fread(dots,sizeof(unsigned char),dotsLen,fp);
  fclose(fp);


  x1=x/4; 
  Set_Column_Address(Shift+x1,Shift+x1+ (font_size / 4) - 1); // 设置列坐标，shift为列偏移量由1322决定。3为24/4-1
  Set_Row_Address(y,y + font_size - 1); 
  Set_Write_RAM();   // 写显存

#ifndef HUD
  for(i = 0;i < dotsLen;i++)
  {
    Con_4_byte(dots[i]);
  }
#else
  for(i = font_size - 1;i>0;i--)
  {
    for(j=0;j < dotsLen / font_size;j++)
    {
      Con_4_byte(dots[i * dotsLen / font_size + j]);
    }
  }
  #endif
}



/**
 * 显示一个英文字符
 * @x: 开始列 范围 0 ~ (256 - font_size)
 * @y: 开始行 0 ~ 63
 * @str: 要显示的字符
 * @font_size: 16 或 24
 */
void Display_1_Asc(unsigned char x,unsigned char y, unsigned char str,unsigned char font_size)
{
  unsigned char x1,i;
  unsigned char dots[72] = {0};
  //unsigned char chinese[2] = {0};
  char fontPath[100] = {0};
  int dots_len = 0;
  //long filelen;
  unsigned char fontWidth = 0;

  FILE *fp;

  x1 = x / 4;


  if(font_size == 16)
  {
    fontWidth = 8;
  }
  else
  {
    fontWidth = 16;
  }

  dots_len = font_size *  fontWidth / 8;


  sprintf(fontPath,"/home/pi/fonts/ASC%d.dot",font_size);

  //打开字库文件
  if((fp = fopen(fontPath,"rb")) == NULL)
  {
    printf("字体文件 %s 打开失败!\n",fontPath);
    return;
  }

  //从点陈字库中读取点陈码
  fseek(fp,0,SEEK_END);
  if(ftell(fp) < (str-0x20) * dots_len + dots_len){
    printf("区位码不正确！");
    fclose(fp);
    return;
  }
  fseek(fp,(str-0x20) * dots_len ,SEEK_SET);
  fread(dots,sizeof(unsigned char),dots_len,fp);
  fclose(fp);

  x1=x/4; 
  Set_Column_Address(Shift+x1,Shift+x1+ (fontWidth / 4) - 1); // 设置列坐标，shift为列偏移量由1322决定。3为24/4-1
  Set_Row_Address(y,y + font_size - 1); 
  Set_Write_RAM();   // 写显存

#ifndef HUD
  for(i = 0;i < dots_len;i++)
  {
    Con_4_byte(dots[i]);
  }
#else
  for(i = font_size - 1;i>0;i--)
  {
    for(j=0;j < dots_len / font_size;j++)
    {
      Con_4_byte(dots[i * dots_len / font_size + j]);
    }
  }
  #endif
}



void Display_Str(int x,int y,char *str,unsigned char font_size)
{
  unsigned char i,len;
  char gb2312[200] = {0};


  u2g(str,strlen(str),gb2312,200);
  len = strlen(gb2312);



  for(i = 0;i<len;i++)
  {

    if(gb2312[i] > 0xA0)
    {
      if(x > (256 - font_size))
      {
        x = 0;
        y += font_size;
      }
      Display_1_Chinese(x,y,&gb2312[i],font_size);
      i ++;
      x += font_size;
    }else
    {
      if(x > (256 - font_size / 2)){
        x = 0;
        y += font_size;
      }

      if(gb2312[i] == '\n'){
        x = 0;
        y += font_size;
        continue;
      }

      Display_1_Asc(x,y,gb2312[i],font_size);
      x += font_size / 2;
    }

    

  }
}

void Display_Number(unsigned char x,unsigned char y,const char *str)
{
  int i,x1,j;
  int len;
  

  len = strlen(str);
  for(i=0;i<len;i++)
  {
    x1 = x / 4;
    Set_Column_Address(Shift+x1,Shift+x1+ (32 / 4) - 1); // 设置列坐标，shift为列偏移量由1322决定。3为24/4-1
    Set_Row_Address(y,y + 64 - 1); 
    Set_Write_RAM();   // 写显存

    for(j=0;j<256;j++){
      if(str[i] == ' '){
        Con_4_byte(0x00);
      }
      else{
        Con_4_byte(Nums[(str[i] - 0x30) * 256 + j]);
      }
      
    }

    x += 32;
  }
}


void Display_Bmp(unsigned char x,unsigned char y,const char *str)
{
  int i,x1;
  int len;
  char s[100] = {0};
  x1 = x / 4;

  strcpy(s,str);
  len = strlen(s);
  for(i=0;i<len;i++)
  {
//    if(str[i] != ' ')
    Show_Pattern(s[i],x1,x1 + 7,y,63);
    x1 +=8;
  }
}
