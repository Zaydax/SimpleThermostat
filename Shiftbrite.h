#include "mbed.h"

//Setup a new class for a Shiftbrite RGB LED module
class Shiftbrite
{
public:
    Shiftbrite(PinName pin_e, PinName pin_l, PinName pin_do, PinName pin_di, PinName pin_clk);
    void write(int red, int green, int blue);

private:
//class sets up the pins
    DigitalOut _pin_e;
    DigitalOut _pin_l;
    SPI _spi;
};

Shiftbrite::Shiftbrite(PinName pin_e, PinName pin_l, PinName pin_do, PinName pin_di, PinName pin_clk)
    : _pin_e(pin_e), _pin_l(pin_l), _spi(pin_do, pin_di, pin_clk)
{
 SPI _spi(pin_do, pin_di, pin_clk);
   _pin_l=0;
   _pin_e=0;
   _spi.format(16,0);
   _spi.frequency(500000);
}

void Shiftbrite::write(int red, int green, int blue)
{
    unsigned int low_color=0;
    unsigned int high_color=0;
    high_color=(blue<<4)|((red&0x3C0)>>6);
    low_color=(((red&0x3F)<<10)|(green));
    _spi.write(high_color);
    _spi.write(low_color);
    _pin_l=1;
    _pin_l=0;
}