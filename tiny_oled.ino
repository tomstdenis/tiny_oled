// tiny OLED library predominantly for rendering text to the display
// can use either the Arduino I2C or TinyWireM

#if 1
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

static void oled_command(struct oled *ol, const unsigned char c)
{
  WIRE.beginTransmission(ol->i2c);
  WIRE.WRITE(0x00);
  WIRE.WRITE(c);
  WIRE.endTransmission();
}

static void oled_commands(struct oled *ol, const unsigned char *c, byte n)
{
  while (n--) oled_command(ol, pgm_read_byte_near(c++));
}

static const byte PROGMEM oled_init_cs[] = {
  SSD1306_DISPLAYOFF,
  SSD1306_SETDISPLAYCLOCKDIV,
  0x80,
  SSD1306_SETMULTIPLEX,
  SSD1306_LCDHEIGHT - 1,
  SSD1306_SETDISPLAYOFFSET,
  0x0,
  SSD1306_SETSTARTLINE | 0x0,
  SSD1306_CHARGEPUMP,
};

static const byte PROGMEM oled_init_2[] = {
  SSD1306_MEMORYMODE,
  0x00,
  SSD1306_SEGREMAP | 0x1,
  SSD1306_COMSCANDEC,

#if defined SSD1306_128_32
  SSD1306_SETCOMPINS,
  0x02,
  SSD1306_SETCONTRAST,
  0x8F,
#elif defined SSD1306_128_64
  SSD1306_SETCOMPINS,
  0x12,
  SSD1306_SETCONTRAST,
#elif defined SSD1306_96_16
  SSD1306_SETCOMPINS,
  0x2,
  SSD1306_SETCONTRAST,
#endif
};

static const byte PROGMEM oled_init_3[] = {
  SSD1306_SETVCOMDETECT,
  0x40,
  SSD1306_DISPLAYALLON_RESUME,
  SSD1306_NORMALDISPLAY,
  SSD1306_DEACTIVATE_SCROLL,
  SSD1306_DISPLAYON,
};

void oled_init(struct oled *ol, byte i2c, int reset, int vccstate)
{
  // I2C Init
  ol->i2c = i2c;
  oled_clear(ol);
  
  WIRE.begin();

  if (reset) {
    // Setup reset pin direction (used by both SPI and I2C)
    pinMode(reset, OUTPUT);
    digitalWrite(reset, HIGH);
    // VDD (3.3V) goes high at start, lets just chill for a ms
    delay(1);
    // bring reset low
    digitalWrite(reset, LOW);
    // wait 10ms
    delay(10);
    // bring out of reset
    digitalWrite(reset, HIGH);
    // turn on VCC (9V?)
  }

  // Init sequence
  oled_commands(ol, oled_init_cs, sizeof(oled_init_cs));

  if (vccstate == SSD1306_EXTERNALVCC) {
    oled_command(ol, 0x10);
  } else {
    oled_command(ol, 0x14);
  }

  oled_commands(ol, oled_init_2, sizeof(oled_init_2));
  
#if defined SSD1306_128_64
  if (vccstate == SSD1306_EXTERNALVCC) {
    oled_command(ol, 0x9F);
  } else {
    oled_command(ol, 0xCF);
  }
#elif defined SSD1306_96_16
  if (vccstate == SSD1306_EXTERNALVCC) {
    oled_command(ol, 0x10);
  } else   {
    oled_command(ol, 0xAF);
  }
#endif

  oled_command(ol, SSD1306_SETPRECHARGE);                  // 0xd9
  if (vccstate == SSD1306_EXTERNALVCC) {
    oled_command(ol, 0x22);
  } else {
    oled_command(ol, 0xF1);
  }
  oled_commands(ol, oled_init_3, sizeof(oled_init_3));
}

// 8x8 font for chars 32..127 in column-major order per char (each char on its own line)
const unsigned char PROGMEM font[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x14, 0x3e, 0x14, 0x3e, 0x14, 0x00, 0x00,
  0x00, 0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12, 0x00,
  0x02, 0x05, 0x15, 0x0a, 0x14, 0x2a, 0x28, 0x10,
  0x00, 0x00, 0x16, 0x29, 0x29, 0x11, 0x10, 0x28,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0c, 0x12, 0x21, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x21, 0x12, 0x0c, 0x00, 0x00,
  0x00, 0x00, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x00,
  0x00, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x40, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00,
  0x00, 0x0c, 0x12, 0x21, 0x21, 0x12, 0x0c, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x3f, 0x20, 0x00, 0x00,
  0x00, 0x22, 0x31, 0x29, 0x29, 0x25, 0x22, 0x00,
  0x00, 0x12, 0x21, 0x29, 0x29, 0x25, 0x12, 0x00,
  0x00, 0x18, 0x14, 0x12, 0x3f, 0x10, 0x00, 0x00,
  0x00, 0x17, 0x25, 0x25, 0x25, 0x25, 0x19, 0x00,
  0x00, 0x1e, 0x25, 0x25, 0x25, 0x25, 0x18, 0x00,
  0x00, 0x21, 0x11, 0x09, 0x05, 0x03, 0x01, 0x00,
  0x00, 0x1a, 0x25, 0x25, 0x25, 0x25, 0x1a, 0x00,
  0x00, 0x06, 0x29, 0x29, 0x29, 0x29, 0x1e, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x40, 0x24, 0x00, 0x00, 0x00,
  0x00, 0x08, 0x08, 0x14, 0x14, 0x22, 0x22, 0x00,
  0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00,
  0x00, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00,
  0x00, 0x02, 0x01, 0x29, 0x09, 0x06, 0x00, 0x00,
  0x18, 0x24, 0x42, 0xba, 0xaa, 0xbe, 0x00, 0x00,
  0x00, 0x30, 0x0c, 0x0b, 0x0b, 0x0c, 0x30, 0x00,
  0x00, 0x3f, 0x25, 0x25, 0x25, 0x1a, 0x00, 0x00,
  0x0c, 0x12, 0x21, 0x21, 0x21, 0x12, 0x00, 0x00,
  0x00, 0x3f, 0x21, 0x21, 0x21, 0x1e, 0x00, 0x00,
  0x00, 0x3f, 0x25, 0x25, 0x25, 0x21, 0x00, 0x00,
  0x00, 0x3f, 0x05, 0x05, 0x05, 0x01, 0x00, 0x00,
  0x0c, 0x12, 0x21, 0x29, 0x29, 0x1a, 0x00, 0x00,
  0x00, 0x3f, 0x04, 0x04, 0x04, 0x04, 0x3f, 0x00,
  0x00, 0x00, 0x21, 0x21, 0x3f, 0x21, 0x21, 0x00,
  0x00, 0x10, 0x20, 0x21, 0x21, 0x1f, 0x00, 0x00,
  0x00, 0x3f, 0x08, 0x0c, 0x12, 0x21, 0x00, 0x00,
  0x00, 0x3f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00,
  0x00, 0x3f, 0x02, 0x04, 0x08, 0x04, 0x02, 0x3f,
  0x00, 0x3f, 0x02, 0x04, 0x08, 0x10, 0x3f, 0x00,
  0x00, 0x00, 0x1e, 0x21, 0x21, 0x21, 0x1e, 0x00,
  0x00, 0x3f, 0x05, 0x05, 0x05, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x1e, 0x21, 0x21, 0x21, 0x5e, 0x00,
  0x00, 0x3f, 0x05, 0x0d, 0x15, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x12, 0x25, 0x29, 0x29, 0x12, 0x00,
  0x00, 0x01, 0x01, 0x01, 0x3f, 0x01, 0x01, 0x01,
  0x00, 0x1f, 0x20, 0x20, 0x20, 0x20, 0x1f, 0x00,
  0x01, 0x06, 0x18, 0x20, 0x20, 0x18, 0x06, 0x01,
  0x00, 0x3f, 0x10, 0x08, 0x04, 0x08, 0x10, 0x3f,
  0x00, 0x21, 0x12, 0x0c, 0x0c, 0x12, 0x21, 0x00,
  0x00, 0x01, 0x02, 0x04, 0x38, 0x04, 0x02, 0x01,
  0x00, 0x21, 0x31, 0x29, 0x25, 0x23, 0x21, 0x00,
  0x00, 0x00, 0x3f, 0x21, 0x21, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00,
  0x00, 0x00, 0x21, 0x21, 0x3f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x01, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x10, 0x2a, 0x2a, 0x2a, 0x1a, 0x3c, 0x00,
  0x00, 0x3f, 0x14, 0x24, 0x24, 0x24, 0x18, 0x00,
  0x00, 0x00, 0x18, 0x24, 0x24, 0x24, 0x00, 0x00,
  0x00, 0x18, 0x24, 0x24, 0x24, 0x14, 0x3f, 0x00,
  0x00, 0x1c, 0x2a, 0x2a, 0x2a, 0x2a, 0x0c, 0x00,
  0x00, 0x00, 0x08, 0x3e, 0x0a, 0x00, 0x00, 0x00,
  0x00, 0x18, 0xa4, 0xa4, 0x88, 0x7c, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0x04, 0x04, 0x38, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x80, 0x84, 0x7d, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0x10, 0x28, 0x24, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x3f, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x3c, 0x04, 0x08, 0x08, 0x04, 0x3c, 0x00,
  0x00, 0x00, 0x3c, 0x08, 0x04, 0x04, 0x3c, 0x00,
  0x00, 0x18, 0x24, 0x24, 0x24, 0x24, 0x18, 0x00,
  0x00, 0xfc, 0x28, 0x24, 0x24, 0x24, 0x18, 0x00,
  0x00, 0x18, 0x24, 0x24, 0x24, 0x28, 0xfc, 0x00,
  0x00, 0x00, 0x3c, 0x08, 0x04, 0x04, 0x08, 0x00,
  0x00, 0x00, 0x24, 0x2a, 0x2a, 0x12, 0x00, 0x00,
  0x00, 0x00, 0x04, 0x3e, 0x24, 0x04, 0x00, 0x00,
  0x00, 0x00, 0x1c, 0x20, 0x20, 0x10, 0x3c, 0x00,
  0x00, 0x0c, 0x10, 0x20, 0x20, 0x10, 0x0c, 0x00,
  0x0c, 0x30, 0x20, 0x10, 0x10, 0x20, 0x30, 0x0c,
  0x00, 0x24, 0x28, 0x10, 0x10, 0x28, 0x24, 0x00,
  0x00, 0x84, 0x88, 0x50, 0x20, 0x10, 0x0c, 0x00,
  0x00, 0x00, 0x24, 0x34, 0x2c, 0x24, 0x00, 0x00,
  0x00, 0x00, 0x0c, 0x3f, 0x21, 0x21, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x21, 0x21, 0x3f, 0x0c, 0x00, 0x00,
  0x00, 0x10, 0x08, 0x08, 0x10, 0x10, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const byte PROGMEM oled_display_sequence[] = {
  SSD1306_COLUMNADDR,
  0,
  SSD1306_LCDWIDTH - 1,
  SSD1306_PAGEADDR,
  0,
#if SSD1306_LCDHEIGHT == 64
  7,
#endif
#if SSD1306_LCDHEIGHT == 32
  3,
#endif
#if SSD1306_LCDHEIGHT == 16
  1,
#endif
  
};
void oled_display(struct oled *ol)
{
  unsigned char tx, ty;
  unsigned short ch = 0;

  oled_commands(ol, oled_display_sequence, sizeof(oled_display_sequence));
  tx = ty = 0;
  for (uint16_t i = 0; i < (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT); i += SSD1306_LCDWIDTH) {
    // send a bunch of data in one xmission
    WIRE.beginTransmission(ol->i2c);
    WIRE.WRITE(0x40);
    for (uint8_t x = 0; x < 16; ) {
      if (!(x & 7)) {
        ch = ol->textbuf[(ty * TXTCOLS) + tx];
        if (ch < 32) ch = 32;
        ch -= 32;
        ch <<= 3;
      }

      WIRE.WRITE(pgm_read_byte_near(font + (ch + (x & 7))));
      ++x;
      if (!(x & 7)) {
        ++tx;
        if (!(tx & 15)) {
          tx = 0;
          ++ty;
        }
      }
    }
    WIRE.endTransmission();
  }
}

void oled_putchar(struct oled *ol, unsigned char ch)
{
  switch (ch) {
    case '\r': ol->tx = 0; break;
    case '\n': ol->ty++; ol->tx = 0; break;
    case 8: if (ol->tx) --(ol->tx); break;
    default:
      ol->textbuf[(ol->ty * TXTCOLS) + ol->tx++] = ch;
      if (ol->tx == 16) {
        ol->tx = 0;
        ++(ol->ty);
      }
      break;
  }
  
  if (ol->ty == TXTROWS) {
    // scroll buffer
    for (unsigned char i = TXTCOLS; i < sizeof(ol->textbuf); i++) ol->textbuf[i-16] = ol->textbuf[i];
    for (unsigned char i = sizeof(ol->textbuf)-TXTCOLS; i < sizeof(ol->textbuf); i++) ol->textbuf[i] = 32;
    ol->ty = TXTROWS-1;
  }
}

void oled_puts(struct oled *ol, const char *msg)
{
  while (*msg) {
    oled_putchar(ol, (unsigned char)*msg++);
  }
}

// can output strings in program memory 
void oled_puts_pgm(struct oled *ol, const char *msg)
{
  unsigned x = 0;
  while (pgm_read_byte_near(msg + x)) {
    oled_putchar(ol, pgm_read_byte_near(msg + x++));
  }
}

void oled_putn(struct oled *ol, int n)
{
  unsigned char buf[8];
  unsigned char i;
  
  if (n < 0) { oled_putchar(ol, '-'); n = -n; }

  i = 0;
  do {
    buf[i++] = '0' + (n % 10);
    n /= 10;
  } while (n);
  
  while (--i != 255) {
    oled_putchar(ol, buf[i]);
  }
  
}

void oled_clear(struct oled *ol)
{
  ol->tx = ol->ty = 0;
  for (unsigned char i = 0; i < sizeof(ol->textbuf); i++) ol->textbuf[i] = 32;
}

void oled_gotoxy(struct oled *ol, unsigned char x, unsigned char y)
{
  ol->tx = x;
  ol->ty = y;
}

 
// demo
struct oled ol;
void setup() {
  // put your setup code here, to run once:
  oled_init(&ol, 0x3D, 4, SSD1306_SWITCHCAPVCC); // pin4 == reset, using I2C address 0x3D}
}

// use SPRINTF in demo or not?
//#define USE_SPRINTF

// use progmem for our string or not (saves RAM but adds ~8 bytes to ROM code)
#define USE_PROGMEM

#ifndef USE_SPRINTF
  #ifdef USE_PROGMEM
  const char PROGMEM buf[] = "\ncnt==";
  #else
  const char buf[] = "\ncnt==";
  #endif
#endif

int cnt=-10;
void loop() {

// demo with counter, also toggle for sprintf vs putn
// sprintf can do a lot but it adds ~1400 bytes to the program whereas
// putn is tiny and can only output signed integers.
// demo on an Trinket Pro drops by 1300 bytes using the putn version
#ifdef USE_SPRINTF
  char buf[16];
  sprintf(buf, "\ncnt==%d", ++cnt);
  oled_puts(&ol, buf);
#else
  #ifdef USE_PROGMEM
    oled_puts_pgm(&ol, buf);
  #else
    oled_puts(&ol, buf);
  #endif
  oled_putn(&ol, ++cnt);
#endif  
  oled_display(&ol);
  delay(500);
}