#include <wave_channel.h>
#include <bus.h>

wave_channel::wave_channel(){
  for(int i = 0; i < 5; i++) registers[i] = 0;
  length_counter = 0;
  divider = 0;
  wave_step = 1;
  power = POWER_OFF;
}

BYTE wave_channel::read_register(wave_audio_register reg){
  return registers[reg];
}

void wave_channel::write_register(wave_audio_register reg, BYTE value){
  registers[reg] = value;
  switch(reg){
    case WAVE_LENGTH_REGISTER:
      length_counter = 256 - value;
      break;

    case WAVE_PERIOD_HIGH_AND_CONTROL_REGISTER:
      if(BIT(value, 7)) trigger();
      break;
  }
}

WORD wave_channel::frequency(){
  WORD low = registers[WAVE_PERIOD_LOW_REGISTER];
  WORD high = registers[WAVE_PERIOD_HIGH_AND_CONTROL_REGISTER] & 0b111;
  return low | (high << 8);
}

BYTE wave_channel::output_level(){
  return (registers[WAVE_OUTPUT_LEVEL_REGISTER] >> 5) & 0b11;
}

bool wave_channel::length_enable(){
  return BIT(registers[WAVE_PERIOD_HIGH_AND_CONTROL_REGISTER], 6);
}

void wave_channel::trigger(){
  power = POWER_ON;
  divider = (2048 - frequency()) * 2;
  if(!length_counter) length_counter = 256 - registers[WAVE_LENGTH_REGISTER];
  wave_step = 1;
}

void wave_channel::length_tick(){
  if(!length_enable()) return;
  length_counter--;
  if(!length_counter) power = POWER_OFF;
}

void wave_channel::tick(){
  if(!BIT(registers[WAVE_DAC_REGISTER], 7)) return;
  if(power == POWER_OFF) return;
  divider--;
  if(!divider){
    divider = (2048 - frequency()) * 2;
    wave_step = (wave_step + 1) % 32;
  }
}

BYTE wave_channel::output(){
  if(power == POWER_OFF || !output_level()) return 0;
  BYTE index = wave_step / 2;
  BYTE out = 0;
  if(wave_step % 2 == 0) out = bus_read(WAVE_RAM_START + index) >> 4;
  else out = bus_read(WAVE_RAM_START + index) & 0xF;

  return out >> (output_level() - 1);
}

