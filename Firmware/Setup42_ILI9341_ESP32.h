// See SetupX_Template.h for all options available
//#define USER_SETUP_ID 42

//define ILI9341_DRIVER VERTICAL
#define ILI9342_DRIVER //HORIZONTAL

#define TFT_MAD_BGR 0x40 // FIX COLORS
#define TFT_RGB_ORDER TFT_RGB


#define TFT_MISO 12  // (leave TFT SDO disconnected if other SPI devices share MISO)
#define TFT_MOSI 13
#define TFT_SCLK 14

#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
//#define TFT_RST   4  // Reset pin (could connect to RST pin)
#define TFT_BL 21
#define TFT_BACKLIGHT_ON HIGH // Enable backlight with gpio level HIGH

#define TFT_SPI_MODE 0
/* NOT WORKING IN MY BOARD
// Optional touch screen chip select
#define TOUCH_CLK 25 // same as TFT_SCLK
#define TOUCH_CS 33 // separate pin for TOUCH_CS
#define TOUCH_DIN 32 // same as TFT_MOSI
#define TOUCH_DO 39 /// same as TFT_MISO

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
*/

// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
//#define SPI_TOUCH_FREQUENCY 2500000 

#define LOAD_GLCD    // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2   // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4   // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6   // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7   // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8   // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF   // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

// TFT SPI clock frequency
// #define SPI_FREQUENCY  20000000
 #define SPI_FREQUENCY  27000000
//#define SPI_FREQUENCY  8000000 // 8Mhz
// #define SPI_FREQUENCY  80000000

// Optional reduced SPI frequency for reading TFT
//#define SPI_READ_FREQUENCY  16000000


