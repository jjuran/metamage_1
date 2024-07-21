#include "Apple-menu.r"

resource 'MENU' (129) {
	2,
	textMenuProc,
	0x7FFFFFC5,
	enabled,
	"File",
	{
		"New Game", noIcon, "N", noMark, plain,
		"Open…", noIcon, "O", noMark, plain,
		"Close", noIcon, "W", noMark, plain,
		"Save", noIcon, "S", noMark, plain,
		"Save As…", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Quit", noIcon, "Q", noMark, plain
	}
};

resource 'MENU' (130) {
	3,
	textMenuProc,
	0x7FFFFFC0,
	enabled,
	"Edit",
	{
		"Undo", noIcon, "Z", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Cut", noIcon, "X", noMark, plain,
		"Copy", noIcon, "C", noMark, plain,
		"Paste", noIcon, "V", noMark, plain,
		"Clear", noIcon, noKey, noMark, plain,
	}
};
