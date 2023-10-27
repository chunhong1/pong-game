struct Button_State {
	bool is_down;
	bool changed;
};

enum {
	BUTTON_UP, //default value is 0 and so on
	BUTTON_DOWN,
	BUTTON_W,
	BUTTON_S,
	BUTTON_A,
	BUTTON_D,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_ENTER,

	BUTTON_COUNT //should be the last item
};

struct Input {
	Button_State buttons[BUTTON_COUNT]; //create an array
};