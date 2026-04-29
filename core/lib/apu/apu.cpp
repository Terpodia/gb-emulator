#include <platform.h>
#include <cstring>
#include <apu/apu.h>

static apu_context ctx;

void apu_init(){
  platform_audio_init(SAMPLE_SIZE);
}

apu_context *apu_get_context(){
  return &ctx;
}

square_audio_register channel1_reg(WORD address){
  return (square_audio_register)(address - 0xFF10);
}

square_audio_register channel2_reg(WORD address){
  return (square_audio_register)(address - 0xFF15);
}

wave_audio_register channel3_reg(WORD address){
  return (wave_audio_register)(address - 0xFF1A);
}

noise_audio_register channel4_reg(WORD address){
  return (noise_audio_register)(address - 0xFF20);
}

BYTE apu_read(WORD address){
  if(address == 0xFF26) return ctx.audio_master_control;

  if(address == 0xFF25) return ctx.sound_panning;

  if(address == 0xFF24) return ctx.master_volume_and_vin_panning;

  if(address >= 0xFF10 && address <= 0xFF14) 
    return ctx.channel1.read_register(channel1_reg(address));

  if(address >= 0xFF16 && address <= 0xFF19) 
    return ctx.channel2.read_register(channel2_reg(address));

  if(address >= 0xFF1A && address <= 0xFF1E) 
    return ctx.channel3.read_register(channel3_reg(address));

  if(address >= 0xFF20 && address <= 0xFF23) 
    return ctx.channel4.read_register(channel4_reg(address));

  if(address >= 0xFF30 && address <= 0xFF3F) 
    return ctx.wave_pattern_ram[address - 0xFF30];

  return 0xFF;
}

void apu_write(WORD address, BYTE value){
  if(address == 0xFF26) BIT_SET(ctx.audio_master_control, 7, value >> 7);

  else if(address == 0xFF25) ctx.sound_panning = value;

  else if(address == 0xFF24) ctx.master_volume_and_vin_panning = value;

  else if(address >= 0xFF10 && address <= 0xFF14) 
    ctx.channel1.write_register(channel1_reg(address), value);

  else if(address >= 0xFF16 && address <= 0xFF19) 
    ctx.channel2.write_register(channel2_reg(address), value);

  else if(address >= 0xFF1A && address <= 0xFF1E) 
    ctx.channel3.write_register(channel3_reg(address), value);

  else if(address >= 0xFF20 && address <= 0xFF23) 
    ctx.channel4.write_register(channel4_reg(address), value);

  else if(address >= 0xFF30 && address <= 0xFF3F) 
    ctx.wave_pattern_ram[address - 0xFF30] = value;
}

void update_audio_master_control(){
  square_channel &channel1 = ctx.channel1;
  square_channel &channel2 = ctx.channel2;
  wave_channel &channel3 = ctx.channel3;
  noise_channel &channel4 = ctx.channel4;

  BIT_SET(ctx.audio_master_control, 0, channel1.power == POWER_ON);
  BIT_SET(ctx.audio_master_control, 1, channel2.power == POWER_ON);
  BIT_SET(ctx.audio_master_control, 2, channel3.power == POWER_ON);
  BIT_SET(ctx.audio_master_control, 3, channel4.power == POWER_ON);
}

void apu_tick(){
  ctx.frame_sequencer_counter++;

  square_channel &channel1 = ctx.channel1;
  square_channel &channel2 = ctx.channel2;
  wave_channel &channel3 = ctx.channel3;
  noise_channel &channel4 = ctx.channel4;

  if(ctx.frame_sequencer_counter == 8192){
    switch(ctx.frame_sequencer){
      case 0:
        channel1.length_tick();
        channel2.length_tick();
        channel3.length_tick();
        channel4.length_tick();
        break;
      case 1:
        break;
      case 2:
        channel1.sweep_tick();

        channel1.length_tick();
        channel2.length_tick();
        channel3.length_tick();
        channel4.length_tick();
        break;
      case 3:
        break;
      case 4:
        channel1.length_tick();
        channel2.length_tick();
        channel3.length_tick();
        channel4.length_tick();
        break;
      case 5:
        break;
      case 6:
        channel1.sweep_tick();

        channel1.length_tick();
        channel2.length_tick();
        channel3.length_tick();
        channel4.length_tick();
        break;
      case 7:
        channel1.envelope_tick();
        channel2.envelope_tick();
        channel4.envelope_tick();
        break;
    }

    ctx.frame_sequencer = (ctx.frame_sequencer + 1) % 8;
    ctx.frame_sequencer_counter = 0;
  }

  ctx.apu_tick++;

  channel1.tick();
  channel2.tick();
  channel3.tick();
  channel4.tick();

  update_audio_master_control();

  if(ctx.apu_tick < 87) return;
  ctx.apu_tick = 0;

  fill_audio_buffer();
}

