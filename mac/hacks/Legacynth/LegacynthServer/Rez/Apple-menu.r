#include "app_name.h"

resource 'MENU' (128) {
	1,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	apple,
	{
		"About " APP_NAME "É", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain
	}
};

