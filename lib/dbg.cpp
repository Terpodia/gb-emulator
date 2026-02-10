#include <dbg.h>
#include <bus.h>

char dbg_msg[1024];
int msg_size = 0;

void dbg_update(){
  if(bus_read(0xFF02) == 0x81){
    dbg_msg[msg_size++] = bus_read(0xFF01);
    bus_write(0xFF02, 0);
  }
}
void dbg_print(){
  if(dbg_msg[0]) std::cout << "DBG: " << dbg_msg << "\n";
}
