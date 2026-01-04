#pragma once
#include <string>
#include <stdint.h>

using namespace std;

class Memory {
public:
    Memory();

    bool load_bin_to_flash(const std::string& path);
    void print_flash(size_t words = 16);
    void toggle_io(uint8_t addr, uint8_t value);
    //metody odczytu z pamięci
    uint16_t fetch_instruction(uint16_t pc);
    uint8_t get_sram(uint16_t cell) const;
    void set_sram(uint16_t cell, uint8_t val);
    uint8_t get_io(uint8_t cell) const;
    void set_io(uint8_t cell, uint8_t val);
    bool get_io_bit(uint8_t cell, uint8_t bit) const;
    void set_io_bit(uint8_t cell, uint8_t bit, bool val);


private:
    uint16_t flash[16 * 1024];
    uint8_t ioreg[64];
    uint8_t ext_ioreg[160];
    uint8_t sram[2048];
};
