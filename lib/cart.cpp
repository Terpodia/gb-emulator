#include <cart.h>
#include <sstream>
#include <iomanip>

static const char *ROM_TYPES[] = {
  "ROM ONLY",
  "MBC1",
  "MBC1+RAM",
  "MBC1+RAM+BATTERY",
  "0x04 ???",
  "MBC2",
  "MBC2+BATTERY",
  "0x07 ???",
  "ROM+RAM 1",
  "ROM+RAM+BATTERY 1",
  "0x0A ???",
  "MMM01",
  "MMM01+RAM",
  "MMM01+RAM+BATTERY",
  "0x0E ???",
  "MBC3+TIMER+BATTERY",
  "MBC3+TIMER+RAM+BATTERY 2",
  "MBC3",
  "MBC3+RAM 2",
  "MBC3+RAM+BATTERY 2",
  "0x14 ???",
  "0x15 ???",
  "0x16 ???",
  "0x17 ???",
  "0x18 ???",
  "MBC5",
  "MBC5+RAM",
  "MBC5+RAM+BATTERY",
  "MBC5+RUMBLE",
  "MBC5+RUMBLE+RAM",
  "MBC5+RUMBLE+RAM+BATTERY",
  "0x1F ???",
  "MBC6",
  "0x21 ???",
  "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

static const char *NEW_LIC_CODE[] = {
  "None",
  "Nintendo R&D1",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Capcom",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Electronic Arts",
  "???",
  "???",
  "???",
  "???",
  "Hudson Soft",
  "b-ai",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "kss",
  "???",
  "pow",
  "???",
  "PCM Complete",
  "san-x",
  "???",
  "???",
  "Kemco Japan",
  "seta",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Viacom",
  "Nintendo",
  "Bandai",
  "Ocean/Acclaim",
  "Konami",
  "Hector",
  "???",
  "Taito",
  "Hudson",
  "Banpresto",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Ubi Soft",
  "Atlus",
  "???",
  "Malibu",
  "???",
  "angel",
  "Bullet-Proof",
  "???",
  "irem",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Absolute",
  "Acclaim",
  "Activision",
  "American sammy",
  "Konami",
  "Hi tech entertainment",
  "LJN",
  "Matchbox",
  "Mattel",
  "Milton Bradley",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Titus",
  "Virgin",
  "???",
  "???",
  "LucasArts",
  "???",
  "???",
  "Ocean",
  "???",
  "Electronic Arts",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Infogrames",
  "Interplay",
  "Broderbund",
  "sculptured",
  "???",
  "sci",
  "???",
  "???",
  "THQ",
  "Accolade",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "misawa",
  "???",
  "???",
  "lozc",
  "???",
  "???",
  "Tokuma Shoten Intermedia",
  "Tsukuda Original",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Chunsoft",
  "Video system",
  "Ocean/Acclaim",
  "???",
  "Varie",
  "Yonezawa/s\'pal",
  "Kaneko",
  "???",
  "Pack in soft",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Konami (Yu-Gi-Oh!)",
};

static const char* LIC_CODE[] = {
  "None",
  "Nintendo",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Capcom",
  "Hot-B",
  "Jaleco",
  "Coconuts Japan",
  "Elite Systems",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "EA (Electronic Arts)",
  "???",
  "???",
  "???",
  "???",
  "Hudsonsoft",
  "ITC Entertainment",
  "Yanoman",
  "???",
  "???",
  "Japan Clary",
  "???",
  "Virgin Interactive",
  "???",
  "???",
  "???",
  "???",
  "PCM Complete",
  "San-X",
  "???",
  "???",
  "Kotobuki Systems",
  "Seta",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Infogrames",
  "Nintendo",
  "Bandai",
  "Indicates that the New licensee code should be used instead.",
  "Konami",
  "HectorSoft",
  "???",
  "???",
  "Capcom",
  "Banpresto",
  "???",
  "???",
  ".Entertainment i",
  "???",
  "Gremlin",
  "???",
  "???",
  "Ubisoft",
  "Atlus",
  "???",
  "Malibu",
  "???",
  "Angel",
  "Spectrum Holoby",
  "???",
  "Irem",
  "Virgin Interactive",
  "???",
  "???",
  "Malibu",
  "???",
  "U.S. Gold",
  "Absolute",
  "Acclaim",
  "Activision",
  "American Sammy",
  "GameTek",
  "Park Place",
  "LJN",
  "Matchbox",
  "???",
  "Milton Bradley",
  "Mindscape",
  "Romstar",
  "Naxat Soft",
  "Tradewest",
  "???",
  "???",
  "Titus",
  "Virgin Interactive",
  "???",
  "???",
  "???",
  "???",
  "???",
  "Ocean Interactive",
  "???",
  "EA (Electronic Arts)",
  "???",
  "???",
  "???",
  "???",
  "Elite Systems",
  "Electro Brain",
  "Infogrames",
  "Interplay",
  "Broderbund",
  "Sculptered Soft",
  "???",
  "The Sales Curve",
  "???",
  "???",
  "t.hq",
  "Accolade",
  "Triffix Entertainment",
  "???",
  "Microprose",
  "???",
  "???",
  "Kemco",
  "Misawa Entertainment",
  "???",
  "???",
  "Lozc",
  "???",
  "???",
  "Tokuma Shoten Intermedia",
  "???",
  "???",
  "???",
  "???",
  "Bullet-Proof Software",
  "Vic Tokai",
  "???",
  "Ape",
  "I'Max",
  "???",
  "Chunsoft Co.",
  "Video System",
  "Tsubaraya Productions Co.",
  "???",
  "Varie Corporation",
  "Yonezawa/S'Pal",
  "Kaneko",
  "???",
  "Arc",
  "Nihon Bussan",
  "Tecmo",
  "Imagineer",
  "Banpresto",
  "???",
  "Nova",
  "???",
  "Hori Electric",
  "Bandai",
  "???",
  "Konami",
  "???",
  "Kawada",
  "Takara",
  "???",
  "Technos Japan",
  "Broderbund",
  "???",
  "Toei Animation",
  "Toho",
  "???",
  "Namco",
  "acclaim",
  "ASCII or Nexsoft",
  "Bandai",
  "???",
  "Square Enix",
  "???",
  "HAL Laboratory",
  "SNK",
  "???",
  "Pony Canyon",
  "Culture Brain",
  "Sunsoft",
  "???",
  "Sony Imagesoft",
  "???",
  "Sammy",
  "Taito",
  "???",
  "Kemco",
  "Squaresoft",
  "Tokuma Shoten Intermedia",
  "Data East",
  "Tonkinhouse",
  "???",
  "Koei",
  "UFL",
  "Ultra",
  "Vap",
  "Use Corporation",
  "Meldac",
  "Pony Canyon or",
  "Angel",
  "Taito",
  "Sofel",
  "Quest",
  "Sigma Enterprises",
  "ASK Kodansha Co.",
  "???",
  "Naxat Soft",
  "Copya System",
  "???",
  "Banpresto",
  "Tomy",
  "LJN",
  "???",
  "NCS",
  "Human",
  "Altron",
  "Jaleco",
  "Towa Chiki",
  "Yutaka",
  "Varie",
  "???",
  "Epcoh",
  "???",
  "Athena",
  "Asmik ACE Entertainment",
  "Natsume",
  "King Records",
  "Atlus",
  "Epic/Sony Records",
  "???",
  "IGS",
  "???",
  "A Wave",
  "???",
  "???",
  "Extreme Entertainment",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "???",
  "LJN",
};

static cart_context ctx;

const char *cart_lic_name() {
  const bool is_unknown_lic = ctx.header->licensee_code >= 0xA5;
  if (is_unknown_lic) return "UNKNOWN";

  const bool use_new_lic = ctx.header->licensee_code == 0x33;
  if (use_new_lic) {
    int ascii_hi = *(ctx.rom_data + 0x144);
    int ascii_low = *(ctx.rom_data + 0x145);
    int hex_hi = (ascii_hi >= 65) ? ascii_hi - 55 : ascii_hi - 48;
    int hex_low = (ascii_low >= 65) ? ascii_low - 55 : ascii_low - 48;
    return NEW_LIC_CODE[hex_hi * 16 + hex_low];
  }
  return LIC_CODE[ctx.header->licensee_code];
}

const char *cart_type_name() {
  if (ctx.header->type < 0x23)
    return ROM_TYPES[ctx.header->type];
  return "UNKNOWN";
}

void print_nintendo_logo(){
  std::string nintendo_logo[0x30];

  for(int i=0; i<0x30; i++){
    std::stringstream stream;
    stream << std::hex << std::setfill('0') << std::setw(2) << (int)ctx.header->logo[i];
    nintendo_logo[i] = std::string(stream.str());
  }

  int t[8][12], offset1=0, offset2=0;
  for(int i=0; i<0x30; i++){
    if(i == 24) offset1 = 4, offset2 = 24;

    int a = ((int)nintendo_logo[i][0] >= 97) ? (int)nintendo_logo[i][0] - 87 : (int)nintendo_logo[i][0] - 48;
    int b = ((int)nintendo_logo[i][1] >= 97) ? (int)nintendo_logo[i][1] - 87 : (int)nintendo_logo[i][1] - 48;

    if(i%2 == 0){
      t[offset1][(i-offset2)/2] = a;
      t[1+offset1][(i-offset2)/2] = b;
    }
    else{
      t[2+offset1][(i-offset2)/2] = a;
      t[3+offset1][(i-offset2)/2] = b;
    }
  }

  for(int i=0; i<8; i++){
    for(int j=0; j<12; j++){
      for(int k=3; k>=0; k--){
        if(BIT(t[i][j], k)) std::cout << "1";
        else std::cout << " ";
      }
    }
    std::cout << "\n";
  }
}

bool cart_load(char *cart_path) {
  snprintf(ctx.filename, sizeof(ctx.filename), "%s", cart_path);
  FILE *fp = fopen(cart_path, "r");
  if (!fp) {
    std::cout << "Failed to open: " << cart_path << "\n";
    return false;
  }
  std::cout << "Opened: " << ctx.filename << "\n";

  fseek(fp, 0, SEEK_END);
  ctx.rom_size = ftell(fp);

  rewind(fp);

  ctx.rom_data = (BYTE*)malloc(ctx.rom_size);
  fread(ctx.rom_data, ctx.rom_size, 1, fp);
  fclose(fp);

  ctx.header = (rom_header*)(ctx.rom_data + 0x100);
  ctx.header->title[15] = 0;

  std::cout << std::hex;

  std::cout << "Cartridge Loaded:\n\n";
  print_nintendo_logo();
  std::cout << "\n";
  std::cout << "\t TITLE       : " << ctx.header->title << "\n";
  std::cout << "\t TYPE        : " << (int)ctx.header->type << " " << " (" << cart_type_name() << ")\n";
  std::cout << "\t LIC_CODE    : " << (int)ctx.header->licensee_code << " " << " (" << cart_lic_name() << ")\n";
  std::cout << "\t ROM SIZE    : " << std::dec << (32 << ctx.header->rom_size) << " KB\n" << std::hex; 
  std::cout << "\t RAM SIZE    : " << (int)ctx.header->ram_size << "\n"; 
  std::cout << "\t ROM VERSION : " << (int)ctx.header->version << "\n";

  WORD checksum = 0;
  for (WORD i=0x0134; i<=0x014C; i++) 
      checksum = checksum - ctx.rom_data[i] - 1;

  if (checksum & 0xFF) 
    std::cout << "\t CHECKSUM    : " << (int)ctx.header->checksum << " (PASSED)\n";
  else 
    std::cout << "\t CHECKSUM    : " << (int)ctx.header->checksum << " (FAILED)\n";

  std::cout << std::dec;
  
  return true;
}

BYTE cart_read(WORD address){
  // for now just ROM ONLY supported
  return ctx.rom_data[address];
}

void cart_write(WORD address, BYTE value){
  // for now just ROM ONLY supported
    std::cout << "Not implemented yet\n";
}