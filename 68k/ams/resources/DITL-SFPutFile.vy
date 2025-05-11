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

let SFPutFile_items =
[
	[[74,  12, 92,  82], button, "Save"  ],
	[[74, 114, 92, 184], button, "Cancel"],
	[[12,  12, 28, 184], text,   ""      ],
	[[16, 209, 34, 295], user,   ""      ],
	[[43, 217, 61, 287], button, "Eject" ],
	[[74, 217, 92, 287], button, "Drive" ],
	[[34,  14, 50, 182], edit,   ""      ],
	[[16, 200, 88, 201], user,   ""      ],
]
