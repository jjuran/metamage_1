let ctrlItem    = 4
let btnCtrl     = 0
let chkCtrl     = 1
let radCtrl     = 2
let resCtrl     = 3
let statText    = 8
let editText    = 16
let iconItem    = 32
let picItem     = 64
let userItem    = 0
let itemDisable = 128

let button = ctrlItem + btnCtrl
let text   = statText + itemDisable
let edit   = editText + itemDisable
let user   = userItem + itemDisable
let live   = userItem

let SFGetFile_items =
[
	[[28,  152,  46,  232], button, "Open"  ],
	[[59, 1152,  77, 1232], button, ""      ],
	[[90,  152, 108,  232], button, "Cancel"],
	[[28,  248,  46,  344], user,   ""      ],
	[[59,  256,  77,  336], button, "Eject" ],
	[[90,  256, 108,  336], button, "Drive" ],
	[[11,   12, 125,  125], live,   ""      ],
	[[11,  124, 125,  140], user,   ""      ],
	[[20,  244, 116,  245], user,   ""      ],
	[[20, 1044, 116, 1145], text,   ""      ],
]
