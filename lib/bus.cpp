#include <bus.h>
#include <cart.h>

BYTE bus_read(WORD address){
    // for now ROM ONLY supported
    return cart_read(address);
}

void bus_write(WORD address, BYTE value){
    // for now ROM ONLY supported
    cart_write(address, value);
}