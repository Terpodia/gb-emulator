#include <apu/apu_utils.h>

int ring_buffer::write(float value){
  int wp = write_pos.load(std::memory_order_relaxed);
  int rp = read_pos.load(std::memory_order_acquire);
  int to_write = std::min(1, capacity - (wp - rp));

  if(to_write > 0) buff[wp & (capacity - 1)] = value;

  write_pos.store(wp + to_write, std::memory_order_release);
  return to_write;
}

int ring_buffer::read(float *data, int sz){
  int rp = read_pos.load(std::memory_order_relaxed);
  int wp = write_pos.load(std::memory_order_acquire);
  int to_read = std::min(sz, wp - rp);

  for(int i = 0; i < to_read; i++)
    data[i] = buff[(rp + i) & (capacity - 1)];

  read_pos.store(rp + to_read, std::memory_order_release);
  return to_read;
}

int ring_buffer::available_space(){
  int rp = read_pos.load(std::memory_order_relaxed);
  int wp = write_pos.load(std::memory_order_relaxed);
  return capacity - (wp - rp);
}
