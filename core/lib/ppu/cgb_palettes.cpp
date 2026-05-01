#include <ppu/cgb_palettes.h>

cgb_palettes_ctx cgb_palettes_ctx;

BYTE cgb_palette_read(WORD address){
  switch(address){
    case 0xff68:
      return cgb_palettes_ctx.bcps;

    case 0xff69:
      return cgb_palettes_ctx.cgb_bg_palette[cgb_palettes_ctx.bcps & 0x3f];

    case 0xff6a:
      return cgb_palettes_ctx.ocps;

    case 0xff6b:
      return cgb_palettes_ctx.cgb_obj_palette[cgb_palettes_ctx.ocps & 0x3f];
  }
  return 0xff;
}

void cgb_palette_write(WORD address, BYTE value){
  switch(address){
    case 0xFF68:
      cgb_palettes_ctx.bcps = value;
      break;

    case 0xFF69:
      cgb_palettes_ctx.cgb_bg_palette[cgb_palettes_ctx.bcps & 0x3F] = value;
      if(BIT(cgb_palettes_ctx.bcps, 7)){
        cgb_palettes_ctx.bcps = (cgb_palettes_ctx.bcps & 0x3F) + 1;
        cgb_palettes_ctx.bcps &= 0x3F;
        cgb_palettes_ctx.bcps |= 1<<7;
      }
      break;

    case 0xFF6A:
      cgb_palettes_ctx.ocps = value;
      break;

    case 0xFF6B:
      cgb_palettes_ctx.cgb_obj_palette[cgb_palettes_ctx.ocps & 0x3F] = value;
      if(BIT(cgb_palettes_ctx.ocps, 7)){
        cgb_palettes_ctx.ocps = (cgb_palettes_ctx.ocps & 0x3F) + 1;
        cgb_palettes_ctx.ocps &= 0x3F;
        cgb_palettes_ctx.ocps |= 1<<7;
      }
      break;
  }
}

uint32_t rgb5_to_rgb8(WORD color){
  uint32_t r = color & 0x1F;
  uint32_t g = (color >> 5) & 0x1F;
  uint32_t b = (color >> 10) & 0x1F;

  r = (r << 3) | (r >> 2);
  g = (g << 3) | (g >> 2);
  b = (b << 3) | (b >> 2);

  return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

uint32_t bg_rgb_color_from_palette(BYTE palette_index, BYTE offset){
  BYTE color_index = COLORS_PER_PALETTE * BYTES_PER_COLOR * palette_index + BYTES_PER_COLOR * offset;
  WORD color = 0;
  color += (WORD)cgb_palettes_ctx.cgb_bg_palette[color_index];
  color += (WORD)cgb_palettes_ctx.cgb_bg_palette[color_index + 1] << 8;

  return rgb5_to_rgb8(color);
}

uint32_t obj_rgb_color_from_palette(BYTE palette_index, BYTE offset){
  BYTE color_index = COLORS_PER_PALETTE * BYTES_PER_COLOR * palette_index + BYTES_PER_COLOR * offset;
  WORD color = 0;
  color += (WORD)cgb_palettes_ctx.cgb_obj_palette[color_index];
  color += (WORD)cgb_palettes_ctx.cgb_obj_palette[color_index + 1] << 8;

  return rgb5_to_rgb8(color);
}

