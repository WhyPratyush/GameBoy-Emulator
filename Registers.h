#pragma once

#include<cstdint>

class Registers{
    private:
        uint8_t a,f;
        uint8_t b,c;
        uint8_t d,e;
        uint8_t h,l;
        uint16_t sp, pc;

    public:

        Registers() {
            reset();
        }

        void reset() {
            set_af(0x01B0);
            set_bc(0x0013);
            set_de(0x00D8);
            set_hl(0x014D);
            sp = 0xFFFE;
            pc = 0x0100;
        }

        uint8_t get_a() const { return a; }
        void set_a(uint8_t val) { a = val; }
        
        uint8_t get_b() const { return b; }
        void set_b(uint8_t val) { b = val; }
        
        uint8_t get_c() const { return c; }
        void set_c(uint8_t val) { c = val; }

        uint8_t get_d() const { return d; }
        void set_d(uint8_t val) { d = val; }

        uint8_t get_e() const { return e; }
        void set_e(uint8_t val) { e = val; }

        uint8_t get_h() const { return h; }
        void set_h(uint8_t val) { h = val; }

        uint8_t get_l() const { return l; }
        void set_l(uint8_t val) { l = val; }

        uint8_t get_f() const { return f; }
        void set_f(uint8_t val) { f = static_cast<uint8_t>(val & 0xF0); }

        uint16_t get_af() const {
        return (a << 8) | f;
    }

    void set_af(uint16_t val) {
        a = static_cast<uint8_t>((val >> 8) & 0xFF); 
        set_f(static_cast<uint8_t>(val & 0xFF));     
    }

    uint16_t get_bc() const {return (b << 8) | c;}
    void set_bc(uint16_t val) {
        b = static_cast<uint8_t>((val >> 8) & 0xFF);
        c = static_cast<uint8_t>(val & 0xFF);
    }

    uint16_t get_de() const {return (d << 8) | e;}
    void set_de(uint16_t val) {
        d = static_cast<uint8_t>((val >> 8) & 0xFF);
        e = static_cast<uint8_t>(val & 0xFF);
    }

    uint16_t get_hl() const { return (h << 8) | l;}
    void set_hl(uint16_t val) {
        h = static_cast<uint8_t>((val >> 8) & 0xFF);
        l = static_cast<uint8_t>(val & 0xFF);
    }

    bool get_z_flag() const { return (f >> 7) & 1; }
    void set_z_flag(bool set) { 
        if (set) f |= 0x80;
        else f &= static_cast<uint8_t>(~0x80); 
    }

    bool get_n_flag() const { return (f >> 6) & 1; }
    void set_n_flag(bool set) { 
        if (set) f |= 0x40; 
        else f &= static_cast<uint8_t>(~0x40); 
    }

    bool get_h_flag() const { return (f >> 5) & 1; }
    void set_h_flag(bool set) { 
        if (set) f |= 0x20; 
        else f &= static_cast<uint8_t>(~0x20); 
    }

    bool get_c_flag() const { return (f >> 4) & 1; }
    void set_c_flag(bool set) { 
        if (set) f |= 0x10; 
        else f &= static_cast<uint8_t>(~0x10); 
    }

    uint16_t get_sp() const { return sp; }
    void set_sp(uint16_t val) { sp = val; }

    uint16_t get_pc() const { return pc; }
    void set_pc(uint16_t val) { pc = val; }
};