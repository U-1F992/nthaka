#include "nxamf.h"
#include "nxamf/pokecon.h"
#include "nxamf/nxmc2.h"

#include <assert.h>
#include <memory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mock_protocol.h"

static void test_mock_protocol(void)
{
    MockProtocol *mock = mock_protocol_new();
    assert(mock != NULL);
    NxamfBytesBuffer *buffer = nxamf_bytes_buffer_new((NxamfBytesProtocolInterface *)mock);
    assert(buffer != NULL);

    assert(nxamf_bytes_buffer_append(buffer, 0) == NULL);
    assert(nxamf_bytes_buffer_append(buffer, 1) == NULL);
    NxamfGamepadState *state = nxamf_bytes_buffer_append(buffer, 2);
    assert(state != NULL);

    assert(state->extension[0] == 0);
    assert(state->extension[1] == 1);
    assert(state->extension[2] == 2);

    nxamf_gamepad_state_delete(state);
    nxamf_bytes_buffer_delete(buffer);
    mock_protocol_delete(mock);
}

static void compare_states(NxamfGamepadState *s1, NxamfGamepadState *s2)
{
    assert(s1->y == s2->y);
    assert(s1->b == s2->b);
    assert(s1->a == s2->a);
    assert(s1->x == s2->x);
    assert(s1->l == s2->l);
    assert(s1->r == s2->r);
    assert(s1->zl == s2->zl);
    assert(s1->zr == s2->zr);
    assert(s1->minus == s2->minus);
    assert(s1->plus == s2->plus);
    assert(s1->l_click == s2->l_click);
    assert(s1->r_click == s2->r_click);
    assert(s1->home == s2->home);
    assert(s1->capture == s2->capture);
    assert(s1->hat == s2->hat);
    assert(s1->l_stick.x == s2->l_stick.x);
    assert(s1->l_stick.y == s2->l_stick.y);
    assert(s1->r_stick.x == s2->r_stick.x);
    assert(s1->r_stick.y == s2->r_stick.y);
    assert(memcmp(s1->extension, s2->extension, 16) == 0);
}

typedef struct TestProtocolParams
{
    uint8_t *data;
    size_t length;
    NxamfGamepadState expected;
} TestProtocolParams;

static void test_pokecon_protocol(void)
{
    PokeConProtocol *protocol = pokecon_protocol_new();
    assert(protocol != NULL);

    TestProtocolParams params[] = {
        {/* 4 8\r\n */ (uint8_t[]){0x34U, 0x20U, 0x38U, 0x0DU, 0x0AU},
         5,
         {/* y */ NXAMF_BUTTON_STATE_PRESSED,
          /* b */ NXAMF_BUTTON_STATE_RELEASED,
          /* a */ NXAMF_BUTTON_STATE_RELEASED,
          /* x */ NXAMF_BUTTON_STATE_RELEASED,
          /* l */ NXAMF_BUTTON_STATE_RELEASED,
          /* r */ NXAMF_BUTTON_STATE_RELEASED,
          /* zl */ NXAMF_BUTTON_STATE_RELEASED,
          /* zr */ NXAMF_BUTTON_STATE_RELEASED,
          /* minus */ NXAMF_BUTTON_STATE_RELEASED,
          /* plus */ NXAMF_BUTTON_STATE_RELEASED,
          /* l_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* r_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* home */ NXAMF_BUTTON_STATE_RELEASED,
          /* capture */ NXAMF_BUTTON_STATE_RELEASED,
          /* hat */ NXAMF_HAT_STATE_NEUTRAL,
          /* l_stick */ {/* x */ NXAMF_STICK_STATE_NEUTRAL,
                         /* y */ NXAMF_STICK_STATE_NEUTRAL},
          /* r_stick */ {/* x */ NXAMF_STICK_STATE_NEUTRAL,
                         /* y */ NXAMF_STICK_STATE_NEUTRAL},
          /* extension */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},

        {/* 2 8 ff 80\r\n */ (uint8_t[]){0x32U, 0x20U, 0x38U, 0x20U, 0x66U, 0x66U, 0x20U, 0x38U, 0x30U, 0x0DU, 0x0AU},
         11,
         {/* y */ NXAMF_BUTTON_STATE_RELEASED,
          /* b */ NXAMF_BUTTON_STATE_RELEASED,
          /* a */ NXAMF_BUTTON_STATE_RELEASED,
          /* x */ NXAMF_BUTTON_STATE_RELEASED,
          /* l */ NXAMF_BUTTON_STATE_RELEASED,
          /* r */ NXAMF_BUTTON_STATE_RELEASED,
          /* zl */ NXAMF_BUTTON_STATE_RELEASED,
          /* zr */ NXAMF_BUTTON_STATE_RELEASED,
          /* minus */ NXAMF_BUTTON_STATE_RELEASED,
          /* plus */ NXAMF_BUTTON_STATE_RELEASED,
          /* l_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* r_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* home */ NXAMF_BUTTON_STATE_RELEASED,
          /* capture */ NXAMF_BUTTON_STATE_RELEASED,
          /* hat */ NXAMF_HAT_STATE_NEUTRAL,
          /* l_stick */ {/* x */ 255,
                         /* y */ NXAMF_STICK_STATE_NEUTRAL},
          /* r_stick */ {/* x */ NXAMF_STICK_STATE_NEUTRAL,
                         /* y */ NXAMF_STICK_STATE_NEUTRAL},
          /* extension */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},

        {/* 0x8003 8 0 0 0 0\r\n */ (uint8_t[]){0x30U, 0x78U, 0x38U, 0x30U, 0x30U, 0x33U, 0x20U, 0x38U, 0x20U, 0x30U, 0x20U, 0x30U, 0x20U, 0x30U, 0x20U, 0x30U, 0x0DU, 0x0AU},
         18,
         {/* y */ NXAMF_BUTTON_STATE_RELEASED,
          /* b */ NXAMF_BUTTON_STATE_RELEASED,
          /* a */ NXAMF_BUTTON_STATE_RELEASED,
          /* x */ NXAMF_BUTTON_STATE_RELEASED,
          /* l */ NXAMF_BUTTON_STATE_RELEASED,
          /* r */ NXAMF_BUTTON_STATE_RELEASED,
          /* zl */ NXAMF_BUTTON_STATE_RELEASED,
          /* zr */ NXAMF_BUTTON_STATE_RELEASED,
          /* minus */ NXAMF_BUTTON_STATE_RELEASED,
          /* plus */ NXAMF_BUTTON_STATE_RELEASED,
          /* l_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* r_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* home */ NXAMF_BUTTON_STATE_RELEASED,
          /* capture */ NXAMF_BUTTON_STATE_PRESSED,
          /* hat */ NXAMF_HAT_STATE_NEUTRAL,
          /* l_stick */ {/* x */ 0,
                         /* y */ 0},
          /* r_stick */ {/* x */ 0,
                         /* y */ 0},
          /* extension */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},

        {/* 3 8 80 80\r\n */ (uint8_t[]){0x33U, 0x20U, 0x38U, 0x20U, 0x38U, 0x30U, 0x20U, 0x38U, 0x30U, 0x0DU, 0x0AU},
         11,
         {/* y */ NXAMF_BUTTON_STATE_RELEASED,
          /* b */ NXAMF_BUTTON_STATE_RELEASED,
          /* a */ NXAMF_BUTTON_STATE_RELEASED,
          /* x */ NXAMF_BUTTON_STATE_RELEASED,
          /* l */ NXAMF_BUTTON_STATE_RELEASED,
          /* r */ NXAMF_BUTTON_STATE_RELEASED,
          /* zl */ NXAMF_BUTTON_STATE_RELEASED,
          /* zr */ NXAMF_BUTTON_STATE_RELEASED,
          /* minus */ NXAMF_BUTTON_STATE_RELEASED,
          /* plus */ NXAMF_BUTTON_STATE_RELEASED,
          /* l_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* r_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* home */ NXAMF_BUTTON_STATE_RELEASED,
          /* capture */ NXAMF_BUTTON_STATE_RELEASED,
          /* hat */ NXAMF_HAT_STATE_NEUTRAL,
          /* l_stick */ {/* x */ 128,
                         /* y */ 128},
          /* r_stick */ {/* x */ 128,
                         /* y */ 128},
          /* extension */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},

        {/* 3 8 80 80 0xb3 0xb\r\n */ (uint8_t[]){0x33U, 0x20U, 0x38U, 0x20U, 0x38U, 0x30U, 0x20U, 0x38U, 0x30U, 0x20U, 0x30U, 0x78U, 0x62U, 0x33U, 0x20, 0x30U, 0x78U, 0x62U, 0x0DU, 0x0AU},
         20,
         {/* y */ NXAMF_BUTTON_STATE_RELEASED,
          /* b */ NXAMF_BUTTON_STATE_RELEASED,
          /* a */ NXAMF_BUTTON_STATE_RELEASED,
          /* x */ NXAMF_BUTTON_STATE_RELEASED,
          /* l */ NXAMF_BUTTON_STATE_RELEASED,
          /* r */ NXAMF_BUTTON_STATE_RELEASED,
          /* zl */ NXAMF_BUTTON_STATE_RELEASED,
          /* zr */ NXAMF_BUTTON_STATE_RELEASED,
          /* minus */ NXAMF_BUTTON_STATE_RELEASED,
          /* plus */ NXAMF_BUTTON_STATE_RELEASED,
          /* l_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* r_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* home */ NXAMF_BUTTON_STATE_RELEASED,
          /* capture */ NXAMF_BUTTON_STATE_RELEASED,
          /* hat */ NXAMF_HAT_STATE_NEUTRAL,
          /* l_stick */ {/* x */ 128,
                         /* y */ 128},
          /* r_stick */ {/* x */ 0xb3,
                         /* y */ 0xb},
          /* extension */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}}};

    for (int i = 0; i < sizeof(params) / sizeof(TestProtocolParams); i++)
    {
        NxamfBytesBuffer *buffer = nxamf_bytes_buffer_new((NxamfBytesProtocolInterface *)protocol);
        assert(buffer != NULL);

        TestProtocolParams p = params[i];
        for (int j = 0; j < p.length - 1; j++)
        {
            assert(nxamf_bytes_buffer_append(buffer, p.data[j]) == NULL);
        }
        NxamfGamepadState *state = nxamf_bytes_buffer_append(buffer, p.data[p.length - 1]);
        assert(state != NULL);
        compare_states(state, &p.expected);

        nxamf_gamepad_state_delete(state);
        nxamf_bytes_buffer_delete(buffer);
    }

    pokecon_protocol_delete(protocol);
}

static void test_nxmc2_protocol(void)
{
    Nxmc2Protocol *protocol = nxmc2_protocol_new();
    assert(protocol != NULL);

    TestProtocolParams params[] = {
        {(uint8_t[]){0xABU, 0x01U, 0x20U, 0x08U, 0x80U, 0x80U, 0x80U, 0x80U, 0x00U, 0x00U, 0x00U},
         11,
         {/* y */ NXAMF_BUTTON_STATE_PRESSED,
          /* b */ NXAMF_BUTTON_STATE_RELEASED,
          /* a */ NXAMF_BUTTON_STATE_RELEASED,
          /* x */ NXAMF_BUTTON_STATE_RELEASED,
          /* l */ NXAMF_BUTTON_STATE_RELEASED,
          /* r */ NXAMF_BUTTON_STATE_RELEASED,
          /* zl */ NXAMF_BUTTON_STATE_RELEASED,
          /* zr */ NXAMF_BUTTON_STATE_RELEASED,
          /* minus */ NXAMF_BUTTON_STATE_RELEASED,
          /* plus */ NXAMF_BUTTON_STATE_RELEASED,
          /* l_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* r_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* home */ NXAMF_BUTTON_STATE_RELEASED,
          /* capture */ NXAMF_BUTTON_STATE_PRESSED,
          /* hat */ NXAMF_HAT_STATE_NEUTRAL,
          /* l_stick */ {/* x */ NXAMF_STICK_STATE_NEUTRAL,
                         /* y */ NXAMF_STICK_STATE_NEUTRAL},
          /* r_stick */ {/* x */ NXAMF_STICK_STATE_NEUTRAL,
                         /* y */ NXAMF_STICK_STATE_NEUTRAL},
          /* extension */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},
        {(uint8_t[]){0xABU, 0x00U, 0x00U, 0x08U, 0x80U, 0x80U, 0x80U, 0x80U, 0x01U, 0x02U, 0x03U},
         11,
         {/* y */ NXAMF_BUTTON_STATE_RELEASED,
          /* b */ NXAMF_BUTTON_STATE_RELEASED,
          /* a */ NXAMF_BUTTON_STATE_RELEASED,
          /* x */ NXAMF_BUTTON_STATE_RELEASED,
          /* l */ NXAMF_BUTTON_STATE_RELEASED,
          /* r */ NXAMF_BUTTON_STATE_RELEASED,
          /* zl */ NXAMF_BUTTON_STATE_RELEASED,
          /* zr */ NXAMF_BUTTON_STATE_RELEASED,
          /* minus */ NXAMF_BUTTON_STATE_RELEASED,
          /* plus */ NXAMF_BUTTON_STATE_RELEASED,
          /* l_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* r_click */ NXAMF_BUTTON_STATE_RELEASED,
          /* home */ NXAMF_BUTTON_STATE_RELEASED,
          /* capture */ NXAMF_BUTTON_STATE_RELEASED,
          /* hat */ NXAMF_HAT_STATE_NEUTRAL,
          /* l_stick */ {/* x */ NXAMF_STICK_STATE_NEUTRAL,
                         /* y */ NXAMF_STICK_STATE_NEUTRAL},
          /* r_stick */ {/* x */ NXAMF_STICK_STATE_NEUTRAL,
                         /* y */ NXAMF_STICK_STATE_NEUTRAL},
          /* extension */ {1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}}};

    for (int i = 0; i < sizeof(params) / sizeof(TestProtocolParams); i++)
    {
        NxamfBytesBuffer *buffer = nxamf_bytes_buffer_new((NxamfBytesProtocolInterface *)protocol);
        assert(buffer != NULL);

        TestProtocolParams p = params[i];
        for (int j = 0; j < p.length - 1; j++)
        {
            assert(nxamf_bytes_buffer_append(buffer, p.data[j]) == NULL);
        }
        NxamfGamepadState *state = nxamf_bytes_buffer_append(buffer, p.data[p.length - 1]);
        assert(state != NULL);
        compare_states(state, &p.expected);

        nxamf_gamepad_state_delete(state);
        nxamf_bytes_buffer_delete(buffer);
    }

    nxmc2_protocol_delete(protocol);
}

int main(void)
{
    test_mock_protocol();
    test_pokecon_protocol();
    test_nxmc2_protocol();

    printf("ok");
    return 0;
}