resource 'MENU' (128, "Apple") {
	1,
	textMenuProc,
	allEnabled,
	enabled,
	apple,
	{	/* array: 1 elements */
		/* [1] */
		"About Pedestal… [abou]", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (129, "File") {
	2,
	textMenuProc,
	0x7FFFFFF7,
	enabled,
	"File",
	{	/* array: 5 elements */
		/* [1] */
		"New [new ]", noIcon, "N", noMark, plain,
		/* [2] */
		"Open… [open]", noIcon, "O", noMark, plain,
		/* [3] */
		"Close [clos]", noIcon, "W", noMark, plain,
		/* [4] */
		"-", noIcon, noKey, noMark, plain,
		/* [5] */
		"Quit [quit]", noIcon, "Q", noMark, plain
	}
};

resource 'MENU' (130, "Edit") {
	3,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	"Edit",
	{	/* array: 7 elements */
		/* [1] */
		"Undo [undo]", noIcon, "Z", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Cut [cut ]", noIcon, "X", noMark, plain,
		/* [4] */
		"Copy [copy]", noIcon, "C", noMark, plain,
		/* [5] */
		"Paste [pste]", noIcon, "V", noMark, plain,
		/* [6] */
		"Clear [clea]", noIcon, noKey, noMark, plain,
		/* [7] */
		"Select All [slct]", noIcon, "A", noMark, plain
	}
};

