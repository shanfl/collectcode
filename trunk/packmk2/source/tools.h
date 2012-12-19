#include <d3dx9.h>

#define PI 3.1415

void debug_render_quad(const LPDIRECT3DDEVICE9 device, const LPDIRECT3DTEXTURE9 texture, int n);
float gaussian(float x, float d);

// add support for logical XOR 

#define log_xor || log_xor_helper() ||

struct log_xor_helper {
    bool value;
};

template<typename LEFT>
log_xor_helper &operator ||(const LEFT &left, log_xor_helper &xor) {
    xor.value = (bool)left;
    return xor;
}

template<typename RIGHT>
bool operator ||(const log_xor_helper &xor, const RIGHT &right) {
    return xor.value ^ (bool)right;
}

