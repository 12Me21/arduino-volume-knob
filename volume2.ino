// connect DT to pin 2
// connect CLK to pin 3
#define BUTTON_PIN  4  // connect SW to any pin
// + isn't needed because the arduino has internal pullup resistors

#define LEFT_KEY  MEDIA_VOLUME_UP
#define RIGHT_KEY  MEDIA_VOLUME_DOWN
#define BUTTON_KEY  MEDIA_PLAY_PAUSE

////////////////////////////////////////////////////////////////////////

#include <HID-Project.h>

// http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
volatile int8_t rot = 0;
void process() {
	enum State : uint8_t {ST=0, L1, L2, L3, R1, R2, R3};
	static State state = ST;
	static const State ttable[7][4] = {
		{0 , L1, R1, ST}, // start (11)
		
		{L2, L1, ST, 0 }, // left 1 (01)
		{L2, L1, L3, 0 }, // left 2 (00)
		{L2, 0 , L3, ST}, // left 3 (10)
		
		{R2, 0 , R1, 0 }, // right 1 (10)
		{R2, R3, R1, 0 }, // right 2 (00)
		{R2, R3, ST, ST}, // right 3 (01)
	};
	uint8_t pinstate = PIND & 0b11;
	
	if (pinstate == 3) {
		if (state == L3)
			rot--;
		else if (state == R3)
			rot++;
	}
	state = ttable[state][pinstate];
}

void setup() {
	// turn off eye-destroying LEDs lol
	pinMode(LED_BUILTIN_TX, INPUT);
	pinMode(LED_BUILTIN_RX, INPUT);
	
	Consumer.begin();
  	
  	pinMode(2, INPUT_PULLUP);
  	pinMode(3, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(2), process, CHANGE);
	attachInterrupt(digitalPinToInterrupt(3), process, CHANGE);
	
	pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
	static int16_t angle;
	angle += rot;
	rot = 0;
	
	if (angle > 50)
		angle = 50;
	else if (angle < -50)
		angle = -50;
	
	if (angle > 0){
		Consumer.write(LEFT_KEY);
		angle--;
	}else if (angle < 0){
		Consumer.write(RIGHT_KEY);
		angle++;
	}
	
	static bool old_pressed;
	bool pressed = digitalRead(BUTTON_PIN);
	if (pressed && ! old_pressed){
		Consumer.write(BUTTON_KEY);
	}
	old_pressed=pressed;
	
	delay(10);
}
