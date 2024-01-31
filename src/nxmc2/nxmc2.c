#include "nxamf/nxmc2.h"
#include "../internal.h"

nxamf_buffer_state_t _update(nxamf_protocol_handler_t *parent, uint8_t d)
{
    nxmc2_protocol_t *ph = (nxmc2_protocol_t *)parent;
    if (ph == NULL)
    {
        return NXAMF_BUFFER_REJECTED;
    }

    switch (ph->_s)
    {
    case NXMC2_PROTOCOL_INITIAL:
        if (d != 0xAB)
            break;
        ph->_s++;
        return NXAMF_BUFFER_PENDING;

    case NXMC2_PROTOCOL_0xAB_0x00:
        if (0x3F < d)
            break;
        ph->_s++;
        return NXAMF_BUFFER_PENDING;

    case NXMC2_PROTOCOL_0xAB_0x00_0x00:
        if (0x08 < d)
            break;
        ph->_s++;
        return NXAMF_BUFFER_PENDING;

    case NXMC2_PROTOCOL_0xAB:
    case NXMC2_PROTOCOL_0xAB_0x00_0x00_0x08:
    case NXMC2_PROTOCOL_0xAB_0x00_0x00_0x08_0x80:
    case NXMC2_PROTOCOL_0xAB_0x00_0x00_0x08_0x80_0x80:
    case NXMC2_PROTOCOL_0xAB_0x00_0x00_0x08_0x80_0x80_0x80:
    case NXMC2_PROTOCOL_0xAB_0x00_0x00_0x08_0x80_0x80_0x80_0x80:
    case NXMC2_PROTOCOL_0xAB_0x00_0x00_0x08_0x80_0x80_0x80_0x80_0x00:
        ph->_s++;
        return NXAMF_BUFFER_PENDING;

    case NXMC2_PROTOCOL_0xAB_0x00_0x00_0x08_0x80_0x80_0x80_0x80_0x00_0x00:
        ph->_s++;
        return NXAMF_BUFFER_ACCEPTED;

    case NXMC2_PROTOCOL_FINAL:
    default:
    }
    return NXAMF_BUFFER_REJECTED;
}

void _reset(nxamf_protocol_handler_t *parent)
{
    nxmc2_protocol_t *ph = (nxmc2_protocol_t *)parent;
    if (ph == NULL)
    {
        return;
    }

    ph->_s = NXMC2_PROTOCOL_INITIAL;
}

bool _deserialize(nxamf_protocol_handler_t *parent, uint8_t *buf, size_t size, nxamf_gamepad_state_t *out)
{
    nxmc2_protocol_t *ph = (nxmc2_protocol_t *)parent;
    if (ph == NULL || ph->_s != NXMC2_PROTOCOL_FINAL)
    {
        return false;
    }

    if (out == NULL)
    {
        // This deserialization has no side effects.
        // If we are not writing to out, there is no need to deserialize in the first place.
        return true;
    }

    uint8_t btns_lsb = buf[NXMC2_BUFFER_INDEX_BUTTONS_LSB];
    out->y = nxamf_internal_bit(btns_lsb, 0);
    out->b = nxamf_internal_bit(btns_lsb, 1);
    out->a = nxamf_internal_bit(btns_lsb, 2);
    out->x = nxamf_internal_bit(btns_lsb, 3);
    out->l = nxamf_internal_bit(btns_lsb, 4);
    out->r = nxamf_internal_bit(btns_lsb, 5);
    out->zl = nxamf_internal_bit(btns_lsb, 6);
    out->zr = nxamf_internal_bit(btns_lsb, 7);

    uint8_t btns_msb = buf[NXMC2_BUFFER_INDEX_BUTTONS_MSB];
    out->minus = nxamf_internal_bit(btns_msb, 0);
    out->plus = nxamf_internal_bit(btns_msb, 1);
    out->l_click = nxamf_internal_bit(btns_msb, 2);
    out->r_click = nxamf_internal_bit(btns_msb, 3);
    out->home = nxamf_internal_bit(btns_msb, 4);
    out->capture = nxamf_internal_bit(btns_msb, 5);

    out->hat = buf[NXMC2_BUFFER_INDEX_HAT];

    out->l_stick.x = buf[NXMC2_BUFFER_INDEX_LX];
    out->l_stick.y = buf[NXMC2_BUFFER_INDEX_LY];
    out->r_stick.x = buf[NXMC2_BUFFER_INDEX_RX];
    out->r_stick.y = buf[NXMC2_BUFFER_INDEX_RY];

    out->extension[0] = buf[NXMC2_BUFFER_INDEX_EXTENSION_0];
    out->extension[1] = buf[NXMC2_BUFFER_INDEX_EXTENSION_1];
    out->extension[2] = buf[NXMC2_BUFFER_INDEX_EXTENSION_2];

    size_t length = sizeof(out->extension) / sizeof(uint8_t);
    for (size_t i = 3; i < length; i++)
    {
        out->extension[i] = 0;
    }

    return true;
}

bool nxmc2_protocol_init(nxmc2_protocol_t *ph)
{
    if (ph == NULL)
    {
        return false;
    }

    ph->parent.deserialize = _deserialize;
    ph->parent.reset = _reset;
    ph->parent.update = _update;

    ph->_s = NXMC2_PROTOCOL_INITIAL;

    return true;
}