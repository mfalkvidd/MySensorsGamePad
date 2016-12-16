#define N_ELEMENTS(array) (sizeof(array)/sizeof((array)[0]))

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RF24_CHANNEL 42
#define MY_RADIO_NRF24

// Make sure we always connect directly to the gateway, to minimize delay
#define MY_PARENT_NODE_ID 0
#define MY_PARENT_NODE_IS_STATIC

// The gateway might not be connected to a controller so we'll use static node id
#define MY_NODE_ID 1

//#define MY_SIGNING_SOFT
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN 7
//#define MY_SIGNING_REQUEST_SIGNATURES

#include <SPI.h>
#include <MySensors.h>

#define SKETCH_NAME "GamePad"
#define SKETCH_MAJOR_VER "1"
#define SKETCH_MINOR_VER "0"

#define DEBOUNCE_MS 5
const byte button_pin[] = {2, 3, 4, 5, 6, 7, 8};
#define NUM_BUTTONS (N_ELEMENTS(button_pin))
MyMessage msg(0, V_TRIPPED);
bool last_button_state[NUM_BUTTONS];

void setup() {
  for (uint8_t button = 0; button < NUM_BUTTONS; button++) {
    pinMode(button_pin[button], INPUT);
    digitalWrite(button_pin[button], HIGH);
  }
  msg.setDestination(0);
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);
  for (byte button = 0; button < NUM_BUTTONS; button++) {
    present(button, S_DOOR);
    wait(50); // Avoid straining the radio too much during presentation
  }
}

void loop() {
  for (uint8_t button = 0; button < NUM_BUTTONS; button++) {
    bool state = !digitalRead(button_pin[button]);
    if (state != last_button_state[button]) {
      // Value has changed from last transmission, send the updated value
      if (send(msg.setSensor(button).set(state))) {
        // Only update local state if we are sure remote state was received
        last_button_state[button] = state;
      }
    }
  }
  wait(DEBOUNCE_MS); // Simple debouncing
}
