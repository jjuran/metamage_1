// Genie.aete.r

#define aeut_RezTemplateVersion 1

#ifndef __AEUSERTERMTYPES_R__
#include "AEUserTermTypes.r"
#endif


resource 'aete' (0)
{
	0x1,
	0x0,
	english,
	roman,
	{	/* array Suites: 2 elements */
		/* [1] */
		"Required Suite",
		"Terms that every application should support",
		'reqd',
		1,
		1,
		{	/* array Events: 0 elements */
		},
		{	/* array Classes: 0 elements */
		},
		{	/* array ComparisonOps: 0 elements */
		},
		{	/* array Enumerations: 0 elements */
		},
		/* [2] */
		"Pipe Organ Suite",
		"Stuff for POSIX-like standard I/O.",
		'|gan',
		1,
		1,
		{	/* array Events: 1 elements */
			/* [1] */
			"execute",
			"Execute a command.",
			'|gan',
			'Exec',
			'long',
			"The result code from running the command.",
			{	/* array: 1 elements */
				/* [1] */
				replyRequired, singleItem, notEnumerated, notTightBindingFunction, enumsAreConstants, enumListCanRepeat, replyIsValue, reserved, reserved, reserved, reserved, reserved, verbEvent, reserved, reserved, reserved
			},
			'****',
			"List of arguments that form the command.",
			{	/* array: 1 elements */
				/* [1] */
				directParamRequired, singleItem, notEnumerated, changesState, enumsAreConstants, enumListCanRepeat, directParamIsValue, directParamIsTarget, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved
			},
			{	/* array OtherParams: 6 elements */
				/* [1] */
				"working from",
				'CWD ',
				'fss ',
				"The working directory.",
				{	/* array: 1 elements */
					/* [1] */
					optional, singleItem, notEnumerated, reserved, enumsAreConstants, enumListCanRepeat, paramIsValue, notParamIsTarget, reserved, reserved, reserved, reserved, prepositionParam, notFeminine, notMasculine, singular
				},
				/* [2] */
				"reading input from",
				'0<  ',
				'****',
				"Input redirection specifier.",
				{	/* array: 1 elements */
					/* [1] */
					optional, singleItem, notEnumerated, reserved, enumsAreConstants, enumListCanRepeat, paramIsValue, notParamIsTarget, reserved, reserved, reserved, reserved, prepositionParam, notFeminine, notMasculine, singular
				},
				/* [3] */
				"with input",
				'0<< ',
				'****',
				"Input sent with the event.",
				{	/* array: 1 elements */
					/* [1] */
					optional, singleItem, notEnumerated, reserved, enumsAreConstants, enumListCanRepeat, paramIsValue, notParamIsTarget, reserved, reserved, reserved, reserved, prepositionParam, notFeminine, notMasculine, singular
				},
				/* [4] */
				"writing output to",
				'1>  ',
				'****',
				"Output redirection specifier.",
				{	/* array: 1 elements */
					/* [1] */
					optional, singleItem, notEnumerated, reserved, enumsAreConstants, enumListCanRepeat, paramIsValue, notParamIsTarget, reserved, reserved, reserved, reserved, prepositionParam, notFeminine, notMasculine, singular
				},
				/* [5] */
				"appending output to",
				'1>> ',
				'****',
				"Output (appending) redirection specifier"
				".",
				{	/* array: 1 elements */
					/* [1] */
					optional, singleItem, notEnumerated, reserved, enumsAreConstants, enumListCanRepeat, paramIsValue, notParamIsTarget, reserved, reserved, reserved, reserved, prepositionParam, notFeminine, notMasculine, singular
				},
				/* [6] */
				"appending",
				'>> ',
				'bool',
				"Whether or not to append to (rather than truncate) the file"
				" to which output is redirected.",
				{	/* array: 1 elements */
					/* [1] */
					optional, singleItem, notEnumerated, reserved, enumsAreConstants, enumListCanRepeat, paramIsValue, notParamIsTarget, reserved, reserved, reserved, reserved, prepositionParam, notFeminine, notMasculine, singular
				}
			}
		},
		{	/* array Classes: 0 elements */
		},
		{	/* array ComparisonOps: 0 elements */
		},
		{	/* array Enumerations: 1 elements */
			/* [1] */
			'I/O ',
			{	/* array Enumerators: 3 elements */
				/* [1] */
				"reply",
				'Repl',
				"The reply Apple event.",
				/* [2] */
				"console",
				'Cons',
				"Console I/O (/dev/console).",
				/* [3] */
				"nowhere",
				'Null',
				"Null I/O (/dev/null)."
			}
		}
	}
};

