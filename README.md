# tiny_oled
This is a demo (eventually a library) to drive the Adafruit 1306 OLED displays in text mode only over I2C only.  It's a very stripped 
down version of the libraries that Adafruit would provide.  I wrote this so I could drive the OLED from an attiny85 or similar 
which isn't possible with the Adafruit library since it would require 18KB of code space and 1.5KB of ram...

Right now this repo only contains a sketch that contains the entire "library".  I'll organize the contents better later on.  

For now you can copy/pasta it into your code just fine.  
