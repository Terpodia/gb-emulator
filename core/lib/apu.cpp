#include "apu.h"
#include <cstring>
#include "platform.h"

const BYTE wave_forms[4] = {0b01111111, 0b00111111, 0b00001111, 0b00000011};

static apu_context ctx = {0};

void apu_init(){
  platform_audio_init(SAMPLE_SIZE);
}

apu_context *apu_get_context(){
  return &ctx;
}

void channel2_tick(){
  if(!CHANNEL2_DAC){
    CHANNEL2_TURN_OFF;
    return;
  }
  ctx.channel2_divider--;
  if(ctx.channel2_divider == 0){
    ctx.channel2_divider = (2048 - CHANNEL2_FREQ) * 4;
    ctx.channel2_wave_step = (ctx.channel2_wave_step + 1) % 8;
  }
}
void channel2_length_tick(){
  if(!CHANNEL2_LENGTH_ENABLE) return;
  ctx.channel2_length++;
  if(ctx.channel2_length >= 64) CHANNEL2_TURN_OFF;
}
void channel2_env_tick(){
  if(!CHANNEL2_ENV_FREQ) return;
  ctx.channel2_envelope--;
  if(!ctx.channel2_envelope){
    ctx.channel2_envelope = CHANNEL2_ENV_FREQ;
    if(CHANNEL2_ENV_DIR && ctx.channel2_volume < 15) ctx.channel2_volume++;
    if(!CHANNEL2_ENV_DIR && ctx.channel2_volume > 0) ctx.channel2_volume--;
  }
}
void channel2_trigger(){
  CHANNEL2_TURN_ON;
  ctx.channel2_divider = (2048 - CHANNEL2_FREQ) * 4;
  ctx.channel2_volume = CHANNEL2_INITIAL_VOLUME;
  if(ctx.channel2_length >= 64) ctx.channel2_length = CHANNEL2_INITIAL_LENGTH;

  ctx.channel2_wave_step = 0;
  ctx.channel2_envelope = CHANNEL2_ENV_FREQ;
}
BYTE channel2_read(WORD address){
  return ctx.channel2_registers[address - 0xFF16];
}
void channel2_write(WORD address, BYTE value){
  if(address == 0xFF19){
    if(value & (1<<7)) channel2_trigger();
  }
  ctx.channel2_registers[address - 0xFF16] = value;
}
BYTE channel2_output(){
  return BIT(wave_forms[CHANNEL2_WAVE_DUTY], ctx.channel2_wave_step) * ctx.channel2_volume;
}

void channel1_tick(){
  if(!CHANNEL1_DAC){
    CHANNEL1_TURN_OFF;
    return;
  }
  ctx.channel1_divider--;
  if(ctx.channel1_divider == 0){
    ctx.channel1_divider = (2048 - CHANNEL1_FREQ) * 4;
    ctx.channel1_wave_step = (ctx.channel1_wave_step + 1) % 8;
  }
}
void channel1_length_tick(){
  if(!CHANNEL1_LENGTH_ENABLE) return;
  ctx.channel1_length++;
  if(ctx.channel1_length >= 64) CHANNEL1_TURN_OFF;
}
void channel1_env_tick(){
  if(!CHANNEL1_ENV_FREQ) return;
  ctx.channel1_envelope--;
  if(!ctx.channel1_envelope){
    ctx.channel1_envelope = CHANNEL1_ENV_FREQ;
    if(CHANNEL1_ENV_DIR && ctx.channel1_volume < 15) ctx.channel1_volume++;
    if(!CHANNEL1_ENV_DIR && ctx.channel1_volume > 0) ctx.channel1_volume--;
  }
}
WORD channel1_sweep_calculation(){
  WORD to_add = ctx.channel1_shadow_register >> CHANNEL1_SWEEP_STEP;
  WORD freq = (CHANNEL1_SWEEP_DIR) ? ctx.channel1_shadow_register - to_add : ctx.channel1_shadow_register + to_add;
  if(freq > 2047) CHANNEL1_TURN_OFF;
  return freq;
}
void channel1_sweep_tick(){
  if(!ctx.channel1_sweep_enable || !CHANNEL1_SWEEP_PACE) return;
  WORD freq = channel1_sweep_calculation();
  if(freq <= 2047){
    ctx.channel1_shadow_register = freq;
    ctx.channel1_registers[3] = freq & 0xFF;
    ctx.channel1_registers[4] = (ctx.channel1_registers[4] & ~0b111)  | (freq >> 8);
    channel1_sweep_calculation();
  }
}
void channel1_trigger(){
  CHANNEL1_TURN_ON;
  ctx.channel1_divider = (2048 - CHANNEL1_FREQ) * 4;
  ctx.channel1_volume = CHANNEL1_INITIAL_VOLUME;
  if(ctx.channel1_length >= 64) ctx.channel1_length = CHANNEL1_INITIAL_LENGTH;
  ctx.channel1_wave_step = 0;
  ctx.channel1_envelope = CHANNEL1_ENV_FREQ;

  ctx.channel1_shadow_register = CHANNEL1_FREQ;
  ctx.channel1_sweep_enable = CHANNEL1_SWEEP_PACE || CHANNEL1_SWEEP_STEP;
  if(CHANNEL1_SWEEP_STEP) channel1_sweep_calculation();
}
BYTE channel1_read(WORD address){
  return ctx.channel1_registers[address - 0xFF10];
}
void channel1_write(WORD address, BYTE value){
  if(address == 0xFF14){
    if(value & (1<<7)) channel1_trigger();
  }
  ctx.channel1_registers[address - 0xFF10] = value;
}
BYTE channel1_output(){
  return BIT(wave_forms[CHANNEL1_WAVE_DUTY], ctx.channel1_wave_step) * ctx.channel1_volume;
}

void channel3_length_tick(){
  if(!CHANNEL3_LENGTH_ENABLE) return;
  ctx.channel3_length++;
  if(!ctx.channel3_length) CHANNEL3_TURN_OFF;
}
void channel3_tick(){
  if(!CHANNEL3_DAC){
    CHANNEL3_TURN_OFF;
    return;
  }
  ctx.channel3_divider--;
  if(ctx.channel3_divider == 0){
    ctx.channel3_divider = (2048 - CHANNEL3_FREQ) * 2;
    ctx.channel3_wave_step = (ctx.channel3_wave_step + 1) % 32;
  }
}
void channel3_trigger(){
  CHANNEL3_TURN_ON;
  ctx.channel3_divider = (2048 - CHANNEL3_FREQ) * 2;
  ctx.channel3_output_level = CHANNEL3_OUTPUT_LEVEL;
  if(!ctx.channel3_length) ctx.channel3_length = CHANNEL3_INITIAL_LENGTH;
  ctx.channel3_wave_step = 1;
}
BYTE channel3_read(WORD address){
  return ctx.channel3_registers[address - 0xFF1A];
}
void channel3_write(WORD address, BYTE value){
  if(address == 0xFF1E){
    if(value & (1<<7)) channel3_trigger();
  }
  ctx.channel3_registers[address - 0xFF1A] = value;
}
BYTE channel3_output(){
  if(!ctx.channel3_output_level) return 0;

  BYTE index = ctx.channel3_wave_step / 2;
  BYTE output = 0;
  if(ctx.channel3_wave_step % 2 == 0) output = ctx.wave_pattern_ram[index] >> 4;
  else output = ctx.wave_pattern_ram[index] & 0xF;

  return output >> (ctx.channel3_output_level - 1);
}

void channel4_tick(){
  if(CHANNEL4_CLOCK_SHIFT >= 14) return;

  if(!CHANNEL4_DAC){
    CHANNEL4_TURN_OFF;
    return;
  }
  ctx.channel4_divider--;
  if(ctx.channel4_divider == 0){
    ctx.channel4_divider = 16;
    if(!CHANNEL4_CLOCK_DIV) ctx.channel4_divider >>= 1;
    else ctx.channel4_divider *= CHANNEL4_CLOCK_DIV;
    ctx.channel4_divider <<= CHANNEL4_CLOCK_SHIFT;

    WORD x = !(BIT(ctx.channel4_lfsr, 0), BIT(ctx.channel4_lfsr, 1));
    BIT_SET(ctx.channel4_lfsr, 15, x);

    if(CHANNEL4_LFSR_WIDTH) BIT_SET(ctx.channel4_lfsr, 7, x);

    ctx.channel4_lfsr >>= 1;
  }
}
void channel4_length_tick(){
  if(!CHANNEL4_LENGTH_ENABLE) return;
  ctx.channel4_length++;
  if(ctx.channel4_length >= 64) CHANNEL4_TURN_OFF;
}
void channel4_env_tick(){
  if(!CHANNEL4_ENV_FREQ) return;
  ctx.channel4_envelope--;
  if(!ctx.channel4_envelope){
    ctx.channel4_envelope = CHANNEL4_ENV_FREQ;
    if(CHANNEL4_ENV_DIR && ctx.channel4_volume < 15) ctx.channel4_volume++;
    if(!CHANNEL4_ENV_DIR && ctx.channel4_volume > 0) ctx.channel4_volume--;
  }
}
void channel4_trigger(){
  CHANNEL4_TURN_ON;

  ctx.channel4_divider = 16;
  if(!CHANNEL4_CLOCK_DIV) ctx.channel4_divider >>= 1;
  else ctx.channel4_divider *= CHANNEL4_CLOCK_DIV;
  ctx.channel4_divider <<= CHANNEL4_CLOCK_SHIFT;

  ctx.channel4_volume = CHANNEL4_INITIAL_VOLUME;
  if(ctx.channel4_length >= 64) ctx.channel4_length = CHANNEL4_INITIAL_LENGTH;
  ctx.channel4_envelope = CHANNEL4_ENV_FREQ;

  ctx.channel4_lfsr = 0;
}
BYTE channel4_read(WORD address){
  return ctx.channel4_registers[address - 0xFF20];
}
void channel4_write(WORD address, BYTE value){
  if(address == 0xFF23){
    if(value & (1<<7)) channel4_trigger();
  }
  ctx.channel4_registers[address - 0xFF20] = value;
}
BYTE channel4_output(){
  return (ctx.channel4_lfsr & 1) * ctx.channel4_volume;
}

BYTE apu_read(WORD address){
  if(address == 0xFF26) return ctx.audio_master_control;
  if(address == 0xFF25) return ctx.sound_panning;
  if(address == 0xFF24) return ctx.master_volume_and_vin_panning;
  if(address >= 0xFF10 && address <= 0xFF14) return channel1_read(address);
  if(address >= 0xFF16 && address <= 0xFF19) return channel2_read(address);
  if(address >= 0xFF1A && address <= 0xFF1E) return channel3_read(address);
  if(address >= 0xFF20 && address <= 0xFF23) return channel4_read(address);
  if(address >= 0xFF30 && address <= 0xFF3F) return ctx.wave_pattern_ram[address - 0xFF30];
  return 0xFF;
}

void apu_write(WORD address, BYTE value){
  if(address == 0xFF26) ctx.audio_master_control = value;
  else if(address == 0xFF25) ctx.sound_panning = value;
  else if(address == 0xFF24) ctx.master_volume_and_vin_panning = value;
  else if(address >= 0xFF10 && address <= 0xFF14) channel1_write(address, value);
  else if(address >= 0xFF16 && address <= 0xFF19) channel2_write(address, value);
  else if(address >= 0xFF1A && address <= 0xFF1E) channel3_write(address, value);
  else if(address >= 0xFF20 && address <= 0xFF23) channel4_write(address, value);
  else if(address >= 0xFF30 && address <= 0xFF3F) ctx.wave_pattern_ram[address - 0xFF30] = value;
}

void apu_tick(){
  ctx.frame_sequencer_counter++;
  if(ctx.frame_sequencer_counter == 8192){
    switch(ctx.frame_sequencer){
      case 0:
        channel1_length_tick();
        channel2_length_tick();
        channel3_length_tick();
        channel4_length_tick();
        break;
      case 1:
        break;
      case 2:
        channel1_sweep_tick();
        channel1_length_tick();
        channel2_length_tick();
        channel3_length_tick();
        channel4_length_tick();
        break;
      case 3:
        break;
      case 4:
        channel1_length_tick();
        channel2_length_tick();
        channel3_length_tick();
        channel4_length_tick();
        break;
      case 5:
        break;
      case 6:
        channel1_sweep_tick();
        channel1_length_tick();
        channel2_length_tick();
        channel3_length_tick();
        channel4_length_tick();
        break;
      case 7:
        channel1_env_tick();
        channel2_env_tick();
        channel4_env_tick();
        break;
    }

    ctx.frame_sequencer = (ctx.frame_sequencer + 1) % 8;
    ctx.frame_sequencer_counter = 0; 
  }

  ctx.apu_tick++;
  if(CHANNEL1_ON) channel1_tick();
  if(CHANNEL2_ON) channel2_tick();
  if(CHANNEL3_ON) channel3_tick();
  if(CHANNEL4_ON) channel4_tick();

  if(ctx.apu_tick < 96) return;
  ctx.apu_tick = 0;

  fill_audio_buffer();

  if(ctx.audio_buffer_index >= SAMPLE_SIZE){
    platform_audio_queue(ctx.audio_buffer, SAMPLE_SIZE);
    ctx.audio_buffer_index = 0;
  } 
}

