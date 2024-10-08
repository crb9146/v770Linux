#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>

// Viper v770 Keyboard Experimental Linux Support

// The Vendor ID and Product ID of the keyboard. This should not change, it uniquely identifies this specific device on any computer.
#define VID 0x04d9
#define PID 0xa12e

// Constants that are present in the URB (USB Request Block) header in the USB protocol.
#define bmRequestType 0x21       // Direction: Host-to-device, Type: Class, Recipient: Interface.
#define bRequest 0x09            // SET_REPORT.
#define wvalue 0x0300            // ReportType: Feature.
#define interface_number 3       // This keyboard takes in RGB control bytes at endpoint 3.
#define timeout 1000             // Arbitrary timeout for keyboard response (after every packet sent from host keyboard sends an "ack" back.
#define control_packet_size 8    // Keyboard always communicates control packets in 8 byte data packets. 
#define RGB_packet_size 64       // Keyboard always RGB control packets in 64 byte data packets.

// When sending control commands, the Windows program always starts and ends with these 8 bytes:
unsigned char start[] = {0x18, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe6};
unsigned char end[] = {0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe7};

libusb_device_handle * initialize_device();
int release_device(libusb_device_handle * handle);
int set_lighting_profile(libusb_device_handle * handle, int profile_num);
int set_lighting_static(libusb_device_handle * handle, int color_num);
int set_lighting_predefined(libusb_device_handle * handle, int predefined_num);
int set_lighting_custom(libusb_device_handle * handle, unsigned char matrix[]);
int send_data(libusb_device_handle * handle, unsigned char data[], uint16_t data_size);

int main() {

    // todo: create a CLI or GUI.
    int profile_to_edit = 3; // change to 1 to 5 based on preferred profile. todo: Add CLI or GUI

    libusb_device_handle * handle;
    int err;

    handle = initialize_device();
    if (!handle){
        // Failed to init device
        return -1;
    }
    
    // profile test setting functions
    set_lighting_profile(handle, profile_to_edit);

    // custom RGB colors setting functions test
    //unsigned char matrix[] = {0x00};
    //set_lighting_custom(handle, matrix);
    
    // Setting static colors testing functions
    int static_color = 3;
    // STATIC COLORS TABLE:
    // 0 = Blink through all
    // 1 = Red
    // 2 = Green
    // 3 = Yellow
    // 4 = Blue
    // 5 = Cyan
    // 6 = Magenta
    // 7 = White
    //set_lighting_static(handle, static_color);

    // setting predefined values testing functions
    int predefined_num = 6;
    // PREDEFINED TABLE
    // 0 = Breathing
    // 1 = Rainbow Wave
    // 2 = Reactive
    // 3 = Sidewinder
    // 4 = Ripple
    // 5 = Unused Reactive alternative? (No "fade out" present in this version).
    // 6 = Spectrum
    // 7 = Unused sidewinder like alternative? Really cool effect. Does not show up in the manual or software.
    // 8 = Raindrop
    // 9 = Vortex
    // 10 = Spotlight
    // 11 = Radar
    //set_lighting_predefined(handle, predefined_num);

    // Release device after program finishes
    release_device(handle);
}

int set_lighting_static(libusb_device_handle * handle, int color_num){
    /* Set a static color on all keys
    The color to load is defined in byte positions 6 and 8
    color_num in position 6 (Ox01 for 1, 0x02 for 2...). Refer to the static color table above
    0xf0 - color_num (so 0xef for 1,...).
    0xbd - color_num (so 0xbc for 1,...).
    */
    
    if (color_num < 0 || color_num > 7){
        printf("Static color number to load must be between 1 and 7!\n");
        return 0;
    }
    
    unsigned char palm_rest[] = {0x07, 0x00, 0x32, 0x04, 0x01, 0xff, 0xff, 0xff}; 
    unsigned char lighting_profile_packet1[] = {0x08, 0x01, 0x01, 0x04, 0x00, color_num, 0x01, 0xf0 - color_num};
    unsigned char lighting_profile_packet2[] = {0x08, 0x02, 0x01, 0x04, 0x32, color_num, 0x01, 0xbd - color_num};
    
    send_data(handle, start, 8);
    send_data(handle, palm_rest, 8);
    send_data(handle, lighting_profile_packet1, 8);
    send_data(handle, lighting_profile_packet2, 8);
    send_data(handle, end, 8);

    return 0;
}

int set_lighting_predefined(libusb_device_handle * handle, int predefined_num){

    /* Set one of the predefined lighting profiles that are included with the keyboard. Ex. Rainbow wave pattern
    The predefined profile to load is defined in byte positions 3 and 8
    0x02 + predefined_num (so 0x03 for 1, 0x04 for 2...).
    0xe7 - predefined_num (so 0xe6 for 1,...).
    0xb4 - predefined_num (so 0xb3 for 1,...).
    */

    // TODO: Find the byte that controls the direction of the effects (such as rainbow wave flow direction)
    
    if (predefined_num < 0 || predefined_num > 11){
        printf("Predefined effect number to load must be between 1 and 11!\n");
        return 0;
    }
    
    unsigned char palm_rest[] = {0x07, 0x00, 0x32, 0x04, 0x01, 0xff, 0xff, 0xff};
    unsigned char lighting_profile_packet1[] = {0x08, 0x01, 0x02 + predefined_num, 0x04, 0x00, 0x08, 0x01, 0xe7 - predefined_num};
    unsigned char lighting_profile_packet2[] = {0x08, 0x02, 0x02 + predefined_num, 0x04, 0x32, 0x08, 0x01, 0xb4 - predefined_num};
    
    send_data(handle, start, 8);
    send_data(handle, palm_rest, 8);
    send_data(handle, lighting_profile_packet1, 8);
    send_data(handle, lighting_profile_packet2, 8);
    send_data(handle, end, 8);
    return 0;
}

int set_lighting_custom( libusb_device_handle * handle, unsigned char matrix[]){
    // Not implemented
    return 0;
}

int set_lighting_profile(libusb_device_handle * handle, int profile_num){
    /* Load one of your custom saved lighting profiles (1 through 5).
    The profile to load is defined in byte positions 3 and 8 in packets lighting_profile_packet1[] and lighting_profile_packet2[]:
    0x32 + profile number (so 0x33 for 1, 0x34 for 2...).
    0xb7 - profile number (so 0xb6 for 1,...).
    0x84 - profile number (so 0x83 for 1,...).
    */
    
    // Safety for making sure the input is between 1 and 5. Keyboard only supports 5 saved profiles.
    if (profile_num < 1 || profile_num > 5){
        printf("Profile number to load must be between 1 and 5!\n");
        return 0;
    }
    
    unsigned char palm_rest[] = {0x07, 0x00, 0x32, 0x04, 0x01, 0xff, 0xff, 0xff}; // Not really sure but I think this is the palmrest colors/config?
    unsigned char lighting_profile_packet1[] = {0x08, 0x01, 0x32 + profile_num, 0x04, 0x00, 0x08, 0x01, 0xb7 - profile_num};

    // uncommenting and sending these two packets "breaks" the keyboard. It will turn off all lights and stop working until you unplug and replug.
    // don't want to brick the keyboard so not messing with this.
    //unsigned char report_config[] = {0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc};
    //unsigned char report_config2[] = {0x04, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2};

    unsigned char lighting_profile_packet2[] = {0x08, 0x02, 0x32 + profile_num, 0x04, 0x32, 0x08, 0x01, 0x84 - profile_num};

    send_data(handle, start, 8);
    send_data(handle, palm_rest, 8);
    send_data(handle, lighting_profile_packet1, 8);
    //send_data(handle, report_config, 8);
    //send_data(handle, report_config2, 8);
    send_data(handle, lighting_profile_packet2, 8);
    send_data(handle, end, 8);
    
    return 0;
}

libusb_device_handle * initialize_device() {
    // Initializes the device and endpoint. Returns the handle to it if successful.

    libusb_device_handle * handle;
    int err; 
    
    // Initialize libusb
    err = libusb_init(NULL);
    if (err < 0) {
        printf("Failed to initialize libusb\n");
        return NULL;
    }

    // Open the device by VID and PID
    // If error here, you need root permissions
    handle = libusb_open_device_with_vid_pid(NULL, VID, PID);
    if (!handle) {
        printf("Failed to open device\n");
        printf("Permission denied. Most likely you forgot sudo.\n");
        libusb_exit(NULL);
        return NULL;
    }

    // Detach the keyboard from the kernel driver. This is so our code and the normal kernel driver will not interfere when running.
    if (libusb_kernel_driver_active(handle, interface_number)) {
        err = libusb_detach_kernel_driver(handle, interface_number);
        if (err != 0) {
            printf("Failed to detach kernel driver: %s\n", libusb_error_name(err));
            libusb_close(handle);
            libusb_exit(NULL);
            return NULL;
        }
    }

    // Claim endpoint interface (3 for this keyboard) so we can communicate with it
    err = libusb_claim_interface(handle, interface_number);
    if (err != 0) {
        printf("Failed to claim interface: %s\n", libusb_error_name(err));
        libusb_close(handle);
        libusb_exit(NULL);
        return NULL;
    }
    return handle;
}

int release_device(libusb_device_handle * handle){
    // Release the interface back to the kernel driver
    // Fixes the keyboard being completely dead upon program exit
    libusb_release_interface(handle, interface_number);
    libusb_attach_kernel_driver(handle, interface_number);
}

int send_data(libusb_device_handle * handle, unsigned char data[], uint16_t data_size) {
    int err = 0;
    
    // Send data 
    err = libusb_control_transfer(handle, bmRequestType, bRequest, wvalue, interface_number, data, data_size, timeout);

    if (err < 0) {
        // if error here, possibly using the wrong type of send function. Sometimes it sends using Interrupts, other times Control.
        printf("Error sending control bytes %s\n", libusb_error_name(err));
        return -1;
    } 
    else {
        printf("Success, sent %d bytes\n", err);
        return err; // return number of bytes successfully sent
    }
}
