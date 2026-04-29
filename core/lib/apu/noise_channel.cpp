#include <apu/noise_channel.h>

noise_channel::noise_channel(){
  for(int i = 0; i < 4; i++) registers[i] = 0;
  length_counter = 0;
  volume_counter = 0;
  volume_envelope_counter = 0;
  divider = 0;
  lfsr = 0;
  power = POWER_OFF;
}

BYTE noise_channel::read_register(noise_audio_register reg){
  return registers[reg];
}

void noise_channel::write_register(noise_audio_register reg, BYTE value){
  registers[reg] = value;
  switch(reg){
    case NOISE_LENGTH_REGISTER:
      length_counter = 64 - (value & 0x3f);
      break;

    case NOISE_CONTROL_REGISTER:
      if(BIT(value, 7)) trigger();

    case NOISE_VOLUME_AND_ENVELOPE_REGISTER:
      break;
  }
}

WORD noise_channel::dac(){
  return registers[NOISE_VOLUME_AND_ENVELOPE_REGISTER] >> 3;
}

BYTE noise_channel::clock_shift(){
  return registers[NOISE_FREQUENCY_AND_RANDOMNESS_REGISTER] >> 4;
}

BYTE noise_channel::clock_divider(){
  return registers[NOISE_FREQUENCY_AND_RANDOMNESS_REGISTER] & 0b111;
}

BYTE noise_channel::lfsr_width(){
  return BIT(registers[NOISE_FREQUENCY_AND_RANDOMNESS_REGISTER], 3);
}

BYTE noise_channel::initial_volume(){
  return registers[NOISE_VOLUME_AND_ENVELOPE_REGISTER] >> 4;
}

BYTE noise_channel::initial_length(){
  return registers[NOISE_LENGTH_REGISTER] & 0x3F;
}

bool noise_channel::length_enable(){
  return BIT(registers[NOISE_CONTROL_REGISTER], 6);
}

void noise_channel::tick(){
  if(!dac()) power = POWER_OFF;
  if(clock_shift() >= 14 || power == POWER_OFF) return;
  divider--;
  if(!divider){
    divider = clock_divider() ? 16 * clock_divider() : 8;
    divider <<= clock_shift();

    WORD b = !(BIT(lfsr, 0) ^ BIT(lfsr, 1));
    BIT_SET(lfsr, 15, b);
    if(lfsr_width()) BIT_SET(lfsr, 7, b);
    lfsr >>= 1;
  }
}

void noise_channel::trigger(){
  power = POWER_ON;
  divider = clock_divider() ? 16 * clock_divider() : 8;
  divider <<= clock_shift();
  volume_counter = initial_volume();
  if(!length_counter) length_counter = 64 - initial_length();
  volume_envelope_counter = envelope_pace();
  lfsr = 0 ;
}

BYTE noise_channel::envelope_pace(){
  return registers[NOISE_VOLUME_AND_ENVELOPE_REGISTER] & 0b111;
}

BYTE noise_channel::envelope_dir(){
  return BIT(registers[NOISE_VOLUME_AND_ENVELOPE_REGISTER], 3);
}

void noise_channel::length_tick(){
  if(!length_enable()) return;
  length_counter--;
  if(!length_counter) power = POWER_OFF;
}

void noise_channel::envelope_tick(){
  if(!envelope_pace()) return;
  volume_envelope_counter--;
  if(!volume_envelope_counter){
    volume_envelope_counter = envelope_pace();
    if(envelope_dir() && volume_counter < 15) volume_counter++;
    if(!envelope_dir() && volume_counter > 0) volume_counter--;
  }
}

BYTE noise_channel::output(){
  return (lfsr & 1) * volume_counter;
}

