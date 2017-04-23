// tiny OLED library predominantly for rendering text to the display
// can use either the Arduino I2C or TinyWireM

#if 0
// normal Arduino I2C
#include <Wire.h>
#define WIRE Wire
#define WRITE write
#else
#include <USI_TWI_Master.h>
#include <TinyWireM.h>
#define WIRE TinyWireM
#define WRITE send
#endif

#define SSD1306_128_64

// some header info
#if defined SSD1306_128_64
#define SSD1306_LCDWIDTH                  128
#define SSD1306_LCDHEIGHT                 64
#endif
#if defined SSD1306_128_32
#define SSD1306_LCDWIDTH                  128
#define SSD1306_LCDHEIGHT                 32
#endif
#if defined SSD1306_96_16
#define SSD1306_LCDWIDTH                  96
#define SSD1306_LCDHEIGHT                 16
#endif

#define TXTCOLS (SSD1306_LCDWIDTH/8)
#define TXTROWS (SSD1306_LCDHEIGHT/8)

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

struct oled {
  unsigned char i2c, tx, ty;
  unsigned char textbuf[TXTROWS * TXTCOLS];
};

void oled_init(struct oled *ol, byte i2c, int reset, int vccstate);
void oled_display(struct oled *ol);
void oled_putchar(struct oled *ol, unsigned char ch);
void oled_puts(struct oled *ol, const char *msg);
void oled_puts_pgm(struct oled *ol, const char *msg);
void oled_putn(struct oled *ol, int n);
void oled_clear(struct oled *ol);
void oled_gotoxy(struct oled *ol, unsigned char x, unsigned char y);
