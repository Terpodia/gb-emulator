#include <apu/square_channel.h>

square_channel::square_channel(){
  for(int i = 0; i < 5; i++) registers[i] = 0;

  length_counter = 0;
  divider = 0;
  wave_step = 0;
  power = POWER_OFF;
  volume_counter = 0;
  volume_envelope_counter = 0;
}

BYTE square_channel::read_register(square_audio_register reg){
  return registers[reg];
}

void square_channel::write_register(square_audio_register reg, BYTE value){
  registers[reg] = value;
  switch(reg){
    case SQUARE_LENGTH_AND_WAVE_DUTY_REGISTER:
      length_counter = 64 - (value & 0x3f);
      break;

    case SQUARE_PERIOD_HIGH_AND_CONTROL_REGISTER:
      if(BIT(value, 7)) trigger();

    case SQUARE_VOLUME_AND_ENVELOPE_REGISTER:
      break;
  }
}

WORD square_channel::frequency(){
  WORD low = registers[SQUARE_PERIOD_LOW_REGISTER];
  WORD high = registers[SQUARE_PERIOD_HIGH_AND_CONTROL_REGISTER] & 0b111;
  return low | (high << 8);
}

WORD square_channel::dac(){
  return registers[SQUARE_VOLUME_AND_ENVELOPE_REGISTER] >> 3;
}

BYTE square_channel::wave_duty(){
  return registers[SQUARE_LENGTH_AND_WAVE_DUTY_REGISTER] >> 6;
}

BYTE square_channel::envelope_pace(){
  return registers[SQUARE_VOLUME_AND_ENVELOPE_REGISTER] & 0b111;
}

BYTE square_channel::envelope_dir(){
  return BIT(registers[SQUARE_VOLUME_AND_ENVELOPE_REGISTER], 3);
}

BYTE square_channel::sweep_pace(){
  return (registers[SQUARE_SWEEP_REGISTER] >> 4) & 0b111;
}

BYTE square_channel::sweep_step(){
  return registers[SQUARE_SWEEP_REGISTER] & 0b111;
}

BYTE square_channel::sweep_dir(){
  return BIT(registers[SQUARE_SWEEP_REGISTER], 3);
}

WORD square_channel::sweep_calculation(){
  WORD to_add = shadow_register >> sweep_step();
  WORD freq = sweep_dir() ? shadow_register - to_add : shadow_register + to_add;
  if(freq > 2047) power = POWER_OFF;
  return freq;
}

BYTE square_channel::initial_volume(){
  return registers[SQUARE_VOLUME_AND_ENVELOPE_REGISTER] >> 4;
}

BYTE square_channel::initial_length(){
  return registers[SQUARE_LENGTH_AND_WAVE_DUTY_REGISTER] & 0x3f;
}

bool square_channel::length_enable(){
  return BIT(registers[SQUARE_PERIOD_HIGH_AND_CONTROL_REGISTER], 6);
}

void square_channel::tick(){
  if(!dac()) power = POWER_OFF;
  if(power == POWER_OFF) return;

  divider--;
  if(!divider){
    divider = (2048 - frequency()) * 4;
    wave_step = (wave_step + 1) % 8;
  }
}

BYTE square_channel::output(){
  if(power == POWER_OFF) return 0;
  return BIT(wave_forms[wave_duty()], wave_step) * volume_counter;
}

void square_channel::trigger(){
  power = POWER_ON;
  divider = (2048 - frequency());
  volume_counter = initial_volume();
  if(!length_counter) length_counter = 64;
  wave_step = 0;
  volume_envelope_counter = envelope_pace();

  shadow_register = frequency();
  sweep_enable = sweep_pace() || sweep_step();
  if(sweep_step()) sweep_calculation();
}

void square_channel::length_tick(){
  if(!length_enable()) return;
  length_counter--;
  if(!length_counter) power = POWER_OFF;
}

void square_channel::envelope_tick(){
  if(!envelope_pace()) return;
  volume_envelope_counter--;
  if(!volume_envelope_counter){
    volume_envelope_counter = envelope_pace();
    if(envelope_dir() && volume_counter < 15) volume_counter++;
    if(!envelope_dir() && volume_counter > 0) volume_counter--;
  }
}

void square_channel::sweep_tick(){
  if(!sweep_enable || !sweep_pace()) return;
  WORD freq = sweep_calculation();
  if(freq <= 2047){
    shadow_register = freq;
    registers[SQUARE_PERIOD_LOW_REGISTER] = freq & 0xFF;
    registers[SQUARE_PERIOD_HIGH_AND_CONTROL_REGISTER] &= ~0b111;
    registers[SQUARE_PERIOD_HIGH_AND_CONTROL_REGISTER] |= freq >> 8;
  }
}

