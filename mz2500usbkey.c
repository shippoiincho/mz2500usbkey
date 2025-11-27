//  MZ-2500 USB KEYBOARD CONVERTER
//
//  Connections:
//  GP0: RSTN
//  GP1: KD0
//  GP2: KD1
//  GP3: KD2
//  GP4: KD3
//  GP5: KD4
//  GP6: MPX

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "mz2500usbkey.h"
#include "tusb.h"
#include "bsp/board.h"

struct repeating_timer timer;

hid_keyboard_report_t prev_report = {0, 0, {0}}; // previous report to check key released
extern void hid_app_task(void);

uint8_t hid_dev_addr = 255;
uint8_t hid_instance = 255;
uint8_t hid_led;

uint8_t keymap[16];

volatile uint32_t timercount=0;
uint32_t lasttimer=0;

bool __not_in_flash_func(timer_handler)(struct repeating_timer *t) {
    timercount++;
    return true;
}


// Keyboard

// static inline bool find_key_in_report(hid_keyboard_report_t const *report, uint8_t keycode)
// {
//   for(uint8_t i=0; i<6; i++)
//   {
//     if (report->keycode[i] == keycode)  return true;
//   }

//   return false;
// }

// void process_kbd_leds(void) {

//     hid_led=0;

//     if(key_caps) hid_led+=KEYBOARD_LED_CAPSLOCK;          // CAPS Lock
//     if(key_kana) hid_led+=KEYBOARD_LED_NUMLOCK;           // KANA -> Numlock
//     if((hid_dev_addr!=255)&&(hid_instance!=255)) {
//         tuh_hid_set_report(hid_dev_addr, hid_instance, 0, HID_REPORT_TYPE_OUTPUT, &hid_led, sizeof(hid_led));
//     }

// }

void process_kbd_report(hid_keyboard_report_t const *report)
{

    int usbkey;
    int anykey;

    for (int i = 0; i < 16; i++) {
        keymap[i] = 0xff;
    }

    // Modifyers

    if (report->modifier & 0x22) { // SHIFT
        keymap[0xb] &= 0xfb;
    }

    if (report->modifier & 0x11) { // CTRL
        keymap[0xb] &= 0xef;
    }

    if (report->modifier & 0x44) { // ALT = GRAPH
        keymap[0xb] &= 0xfe;
    }

    //

    for (int i = 0; i < 6; i++) {

        if (report->keycode[i]) {
            usbkey = report->keycode[i];
            if (mzusbcode[usbkey * 2]) {
                keymap[mzusbcode[usbkey * 2 + 1]] &= ~mzusbcode[usbkey * 2];
            }
        }
    }

    prev_report = *report;

    //

    anykey = keymap[0];

    for (int i = 1; i < 0xe; i++) {
        anykey &= keymap[i];
    }
    keymap[0xf] = anykey;
}

int main()
{

    uint32_t bus;
    uint8_t scanline;
    uint8_t mode;

    for(int i=0;i<16;i++) {
        keymap[i]=0xff;
    }

    add_repeating_timer_us(1000000/60,timer_handler,NULL  ,&timer);
    tuh_init(BOARD_TUH_RHPORT);

    gpio_init_mask(0xff);
//    gpio_init(25);
//    gpio_set_dir(25,1);

    scanline=0;
    mode=0;

    while (true) {

        bus = gpio_get_all();

        if (bus & 1) { 
            if(mode!=0) {
                gpio_set_dir_masked(0x1e, 0);
                bus = gpio_get_all();
            }            
            // Receive KD
            scanline = (bus & 0x1e) >> 1;
            mode=0;
        } else {

            if(mode==0) {
                // set GP1-4 to OUTPUT
                gpio_set_dir_masked(0x1e, 0x1e);
            }

        //    if(keymap[scanline]!=0xff) {
        //        gpio_put(25,1);
        //    }


            // MPX

            if(bus & 0x40 ) { // Upper nibble

                if(mode==0) {
                    if (bus & 0x20) { // Send
                        gpio_put_masked(0x1e,(keymap[scanline]&0xf0)>>3);
                    } else {
                        gpio_put_masked(0x1e,(keymap[0xf]&0xf0)>>3);
                    }
                mode=1;
                }


            } else { // Lower nibble

                if(mode==1) {
                    if (bus & 0x20) {
                        gpio_put_masked(0x1e,(keymap[scanline]&0xf)<<1);
                    } else {
                        gpio_put_masked(0x1e,(keymap[0xf]&0xf)<<1);
                    }
                    mode=2;
                }

            }
        }

        if(timercount!=lasttimer) {
            tuh_task();
            lasttimer=timercount;
        }


    }
}
