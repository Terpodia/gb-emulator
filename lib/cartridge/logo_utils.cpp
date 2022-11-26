#include <cartridge/cart.h>

#include <iomanip>
#include <sstream>
#include <vector>

std::vector<std::string> convert_logo_to_hex(BYTE logo[0x30]) {
  std::vector<std::string> nintendo_logo(0x30);

  for (int i = 0; i < 0x30; i++) {
    std::stringstream stream;
    stream << std::hex << std::setfill('0') << std::setw(2) << (int)logo[i];
    nintendo_logo[i] = std::string(stream.str());
  }

  return nintendo_logo;
}

std::vector<std::vector<int>> decode_nintendo_logo(
    std::vector<std::string> nintendo_logo) {
  std::vector<std::vector<int>> t(8, std::vector<int>(12));
  int offset1 = 0, offset2 = 0;
  for (int i = 0; i < 0x30; i++) {
    if (i == 24) offset1 = 4, offset2 = 24;

    int a = ((int)nintendo_logo[i][0] >= 97) ? (int)nintendo_logo[i][0] - 87
                                             : (int)nintendo_logo[i][0] - 48;
    int b = ((int)nintendo_logo[i][1] >= 97) ? (int)nintendo_logo[i][1] - 87
                                             : (int)nintendo_logo[i][1] - 48;

    if (i % 2 == 0) {
      t[offset1][(i - offset2) / 2] = a;
      t[1 + offset1][(i - offset2) / 2] = b;
    } else {
      t[2 + offset1][(i - offset2) / 2] = a;
      t[3 + offset1][(i - offset2) / 2] = b;
    }
  }
  return t;
}

void print_logo(std::vector<std::vector<int>> logo) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 12; j++) {
      for (int k = 3; k >= 0; k--) {
        if (BIT(logo[i][j], k))
          std::cout << "1";
        else
          std::cout << " ";
      }
    }
    std::cout << "\n";
  }
}

void decode_and_print_logo(BYTE logo[0x30]) {
  std::vector<std::string> logo_encoded = convert_logo_to_hex(logo);
  std::vector<std::vector<int>> logo_decoded =
      decode_nintendo_logo(logo_encoded);
  print_logo(logo_decoded);
}
