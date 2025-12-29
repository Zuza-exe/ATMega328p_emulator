#pragma once
#include <string>
#include <stdint.h>

using namespace std;

class Memory {
public:
    Memory();

    bool load_bin_to_flash(const std::string& path);
    void print_flash(size_t words = 16);
    void write_io(uint8_t addr, uint8_t value);
    //metody odczytu z pamięci

private:
    uint16_t flash[16 * 1024];
    uint8_t ioreg[64];
    uint8_t ext_ioreg[160];
    uint8_t sram[2048];
};
