/*
	Public domain by Andrew M. <liquidsun@gmail.com>
	See: https://github.com/floodyberry/curve25519-donna

	32 bit integer curve25519 implementation
*/

#define curve25519_add_after_basic curve25519_add_reduce
#define curve25519_sub_after_basic curve25519_sub_reduce

typedef uint32_t bignum25519[10];
typedef uint32_t bignum25519align16[12];

static const uint32_t reduce_mask_25 = (1 << 25) - 1;
static const uint32_t reduce_mask_26 = (1 << 26) - 1;


/* out = in */
DONNA_INLINE static void
curve25519_copy(bignum25519 out, const bignum25519 in) {
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
	out[3] = in[3];
	out[4] = in[4];
	out[5] = in[5];
	out[6] = in[6];
	out[7] = in[7];
	out[8] = in[8];
	out[9] = in[9];
}

/* out = a + b */
static void
curve25519_add(bignum25519 out, const bignum25519 a, const bignum25519 b) {
	out[0] = a[0] + b[0];
	out[1] = a[1] + b[1];
	out[2] = a[2] + b[2];
	out[3] = a[3] + b[3];
	out[4] = a[4] + b[4];
	out[5] = a[5] + b[5];
	out[6] = a[6] + b[6];
	out[7] = a[7] + b[7];
	out[8] = a[8] + b[8];
	out[9] = a[9] + b[9];
}

static void
curve25519_add_reduce(bignum25519 out, const bignum25519 a, const bignum25519 b) {
	uint32_t c;
	out[0] = a[0] + b[0]    ; c = (out[0] >> 26); out[0] &= reduce_mask_26;
	out[1] = a[1] + b[1] + c; c = (out[1] >> 25); out[1] &= reduce_mask_25;
	out[2] = a[2] + b[2] + c; c = (out[2] >> 26); out[2] &= reduce_mask_26;
	out[3] = a[3] + b[3] + c; c = (out[3] >> 25); out[3] &= reduce_mask_25;
	out[4] = a[4] + b[4] + c; c = (out[4] >> 26); out[4] &= reduce_mask_26;
	out[5] = a[5] + b[5] + c; c = (out[5] >> 25); out[5] &= reduce_mask_25;
	out[6] = a[6] + b[6] + c; c = (out[6] >> 26); out[6] &= reduce_mask_26;
	out[7] = a[7] + b[7] + c; c = (out[7] >> 25); out[7] &= reduce_mask_25;
	out[8] = a[8] + b[8] + c; c = (out[8] >> 26); out[8] &= reduce_mask_26;
	out[9] = a[9] + b[9] + c; c = (out[9] >> 25); out[9] &= reduce_mask_25;
	out[0] += 19 * c;
}

/* multiples of p */
static const uint32_t twoP0       = 0x07ffffda;
static const uint32_t twoP13579   = 0x03fffffe;
static const uint32_t twoP2468    = 0x07fffffe;
static const uint32_t fourP0      = 0x0fffffb4;
static const uint32_t fourP13579  = 0x07fffffc;
static const uint32_t fourP2468   = 0x0ffffffc;

/* out = a - b */
static void
curve25519_sub(bignum25519 out, const bignum25519 a, const bignum25519 b) {
	uint32_t c;
	out[0] = twoP0     + a[0] - b[0]    ; c = (out[0] >> 26); out[0] &= reduce_mask_26;
	out[1] = twoP13579 + a[1] - b[1] + c; c = (out[1] >> 25); out[1] &= reduce_mask_25;
	out[2] = twoP2468  + a[2] - b[2] + c; c = (out[2] >> 26); out[2] &= reduce_mask_26;
	out[3] = twoP13579 + a[3] - b[3] + c; c = (out[3] >> 25); out[3] &= reduce_mask_25;
	out[4] = twoP2468  + a[4] - b[4] + c;
	out[5] = twoP13579 + a[5] - b[5]    ;
	out[6] = twoP2468  + a[6] - b[6]    ;
	out[7] = twoP13579 + a[7] - b[7]    ;
	out[8] = twoP2468  + a[8] - b[8]    ;
	out[9] = twoP13579 + a[9] - b[9]    ;
}

/* out = a - b, where a is the result of a basic op (add,sub) */
static void
curve25519_sub_reduce(bignum25519 out, const bignum25519 a, const bignum25519 b) {
	uint32_t c;
	out[0] = fourP0     + a[0] - b[0]    ; c = (out[0] >> 26); out[0] &= reduce_mask_26;
	out[1] = fourP13579 + a[1] - b[1] + c; c = (out[1] >> 25); out[1] &= reduce_mask_25;
	out[2] = fourP2468  + a[2] - b[2] + c; c = (out[2] >> 26); out[2] &= reduce_mask_26;
	out[3] = fourP13579 + a[3] - b[3] + c; c = (out[3] >> 25); out[3] &= reduce_mask_25;
	out[4] = fourP2468  + a[4] - b[4] + c; c = (out[4] >> 26); out[4] &= reduce_mask_26;
	out[5] = fourP13579 + a[5] - b[5] + c; c = (out[5] >> 25); out[5] &= reduce_mask_25;
	out[6] = fourP2468  + a[6] - b[6] + c; c = (out[6] >> 26); out[6] &= reduce_mask_26;
	out[7] = fourP13579 + a[7] - b[7] + c; c = (out[7] >> 25); out[7] &= reduce_mask_25;
	out[8] = fourP2468  + a[8] - b[8] + c; c = (out[8] >> 26); out[8] &= reduce_mask_26;
	out[9] = fourP13579 + a[9] - b[9] + c; c = (out[9] >> 25); out[9] &= reduce_mask_25;
	out[0] += 19 * c;
}

/* out = -a */
static void
curve25519_neg(bignum25519 out, const bignum25519 a) {
	uint32_t c;
	out[0] = twoP0     - a[0]    ; c = (out[0] >> 26); out[0] &= reduce_mask_26;
	out[1] = twoP13579 - a[1] + c; c = (out[1] >> 25); out[1] &= reduce_mask_25;
	out[2] = twoP2468  - a[2] + c; c = (out[2] >> 26); out[2] &= reduce_mask_26;
	out[3] = twoP13579 - a[3] + c; c = (out[3] >> 25); out[3] &= reduce_mask_25;
	out[4] = twoP2468  - a[4] + c; c = (out[4] >> 26); out[4] &= reduce_mask_26;
	out[5] = twoP13579 - a[5] + c; c = (out[5] >> 25); out[5] &= reduce_mask_25;
	out[6] = twoP2468  - a[6] + c; c = (out[6] >> 26); out[6] &= reduce_mask_26;
	out[7] = twoP13579 - a[7] + c; c = (out[7] >> 25); out[7] &= reduce_mask_25;
	out[8] = twoP2468  - a[8] + c; c = (out[8] >> 26); out[8] &= reduce_mask_26;
	out[9] = twoP13579 - a[9] + c; c = (out[9] >> 25); out[9] &= reduce_mask_25;
	out[0] += 19 * c;
}

/* out = a * b */
#define curve25519_mul_noinline curve25519_mul
static void
curve25519_mul(bignum25519 out, const bignum25519 a, const bignum25519 b) {
	uint32_t r[ 10 ];
	uint32_t s[ 10 ];
	uint64_t m[ 10 ], c;
	uint32_t p;
	int i, j;

	r[0] = b[0];
	r[1] = b[1];
	r[2] = b[2];
	r[3] = b[3];
	r[4] = b[4];
	r[5] = b[5];
	r[6] = b[6];
	r[7] = b[7];
	r[8] = b[8];
	r[9] = b[9];

	s[0] = a[0];
	s[1] = a[1];
	s[2] = a[2];
	s[3] = a[3];
	s[4] = a[4];
	s[5] = a[5];
	s[6] = a[6];
	s[7] = a[7];
	s[8] = a[8];
	s[9] = a[9];

	for ( i = 1;  i < 10;  i += 2 )
	{
		m[i] = 0;
		
		for ( j = 0;  j <= i;  ++j )
		{
			m[i] += mul32x32_64(r[j], s[i - j]);
		}
	}
	
	r[1] *= 2;
	r[3] *= 2;
	r[5] *= 2;
	r[7] *= 2;

	for ( i = 0;  i < 10;  i += 2 )
	{
		m[i] = 0;
		
		for ( j = 0;  j <= i;  ++j )
		{
			m[i] += mul32x32_64(r[j], s[i - j]);
		}
	}
	
	r[1] *= 19;
	r[2] *= 19;
	r[3] = (r[3] / 2) * 19;
	r[4] *= 19;
	r[5] = (r[5] / 2) * 19;
	r[6] *= 19;
	r[7] = (r[7] / 2) * 19;
	r[8] *= 19;
	r[9] *= 19;

	for ( i = 1;  i < 9;  i += 2 )
	{
		for ( j = 9;  j > i;  --j )
		{
			m[i] += mul32x32_64(r[j], s[10 + i - j]);
		}
	}
	
	r[3] *= 2;
	r[5] *= 2;
	r[7] *= 2;
	r[9] *= 2;

	m[0] += (mul32x32_64(r[9], s[1]) + mul32x32_64(r[8], s[2]) + mul32x32_64(r[7], s[3]) + mul32x32_64(r[6], s[4]) + mul32x32_64(r[5], s[5]) + mul32x32_64(r[4], s[6]) + mul32x32_64(r[3], s[7]) + mul32x32_64(r[2], s[8]) + mul32x32_64(r[1], s[9]));
	m[2] += (mul32x32_64(r[9], s[3]) + mul32x32_64(r[8], s[4]) + mul32x32_64(r[7], s[5]) + mul32x32_64(r[6], s[6]) + mul32x32_64(r[5], s[7]) + mul32x32_64(r[4], s[8]) + mul32x32_64(r[3], s[9]));
	m[4] += (mul32x32_64(r[9], s[5]) + mul32x32_64(r[8], s[6]) + mul32x32_64(r[7], s[7]) + mul32x32_64(r[6], s[8]) + mul32x32_64(r[5], s[9]));
	m[6] += (mul32x32_64(r[9], s[7]) + mul32x32_64(r[8], s[8]) + mul32x32_64(r[7], s[9]));
	m[8] += (mul32x32_64(r[9], s[9]));

	                                 r[0] = (uint32_t)m[0] & reduce_mask_26; c = (m[0] >> 26);
	m[1] += c;                       r[1] = (uint32_t)m[1] & reduce_mask_25; c = (m[1] >> 25);
	m[2] += c;                       r[2] = (uint32_t)m[2] & reduce_mask_26; c = (m[2] >> 26);
	m[3] += c;                       r[3] = (uint32_t)m[3] & reduce_mask_25; c = (m[3] >> 25);
	m[4] += c;                       r[4] = (uint32_t)m[4] & reduce_mask_26; c = (m[4] >> 26);
	m[5] += c;                       r[5] = (uint32_t)m[5] & reduce_mask_25; c = (m[5] >> 25);
	m[6] += c;                       r[6] = (uint32_t)m[6] & reduce_mask_26; c = (m[6] >> 26);
	m[7] += c;                       r[7] = (uint32_t)m[7] & reduce_mask_25; c = (m[7] >> 25);
	m[8] += c;                       r[8] = (uint32_t)m[8] & reduce_mask_26; c = (m[8] >> 26);
	m[9] += c;                       r[9] = (uint32_t)m[9] & reduce_mask_25; p = (uint32_t)(m[9] >> 25);
	m[0] = r[0] + mul32x32_64(p,19); r[0] = (uint32_t)m[0] & reduce_mask_26; p = (uint32_t)(m[0] >> 26);
	r[1] += p;

	out[0] = r[0];
	out[1] = r[1];
	out[2] = r[2];
	out[3] = r[3];
	out[4] = r[4];
	out[5] = r[5];
	out[6] = r[6];
	out[7] = r[7];
	out[8] = r[8];
	out[9] = r[9];
}

/* r = r*r */
static void
curve25519_square_in_place( bignum25519 r )
{
	#define D(i_6)  d[i_6 - 6]
	
	uint32_t d[4];
	uint64_t m[10], c;
	uint32_t p;

	m[0] = mul32x32_64(r[0], r[0]);
	r[0] *= 2;
	m[1] = mul32x32_64(r[0], r[1]);
	m[2] = mul32x32_64(r[0], r[2]) + mul32x32_64(r[1], r[1] * 2);
	r[1] *= 2;
	m[3] = mul32x32_64(r[0], r[3]) + mul32x32_64(r[1], r[2]    );
	m[4] = mul32x32_64(r[0], r[4]) + mul32x32_64(r[1], r[3] * 2) + mul32x32_64(r[2], r[2]);
	r[2] *= 2;
	m[5] = mul32x32_64(r[0], r[5]) + mul32x32_64(r[1], r[4]    ) + mul32x32_64(r[2], r[3]);
	m[6] = mul32x32_64(r[0], r[6]) + mul32x32_64(r[1], r[5] * 2) + mul32x32_64(r[2], r[4]) + mul32x32_64(r[3], r[3] * 2);
	r[3] *= 2;
	m[7] = mul32x32_64(r[0], r[7]) + mul32x32_64(r[1], r[6]    ) + mul32x32_64(r[2], r[5]) + mul32x32_64(r[3], r[4]    );
	m[8] = mul32x32_64(r[0], r[8]) + mul32x32_64(r[1], r[7] * 2) + mul32x32_64(r[2], r[6]) + mul32x32_64(r[3], r[5] * 2) + mul32x32_64(r[4], r[4]    );
	m[9] = mul32x32_64(r[0], r[9]) + mul32x32_64(r[1], r[8]    ) + mul32x32_64(r[2], r[7]) + mul32x32_64(r[3], r[6]    ) + mul32x32_64(r[4], r[5] * 2);

	D(6) = r[6] * 19;
	D(7) = r[7] * 2 * 19;
	D(8) = r[8] * 19;
	D(9) = r[9] * 2 * 19;

	m[0] += (mul32x32_64(D(9), r[1]    ) + mul32x32_64(D(8), r[2]    ) + mul32x32_64(D(7), r[3]    ) + mul32x32_64(D(6), r[4] * 2) + mul32x32_64(r[5], r[5] * 2 * 19));
	m[1] += (mul32x32_64(D(9), r[2] / 2) + mul32x32_64(D(8), r[3]    ) + mul32x32_64(D(7), r[4]    ) + mul32x32_64(D(6), r[5] * 2));
	m[2] += (mul32x32_64(D(9), r[3]    ) + mul32x32_64(D(8), r[4] * 2) + mul32x32_64(D(7), r[5] * 2) + mul32x32_64(D(6), r[6]    ));
	m[3] += (mul32x32_64(D(9), r[4]    ) + mul32x32_64(D(8), r[5] * 2) + mul32x32_64(D(7), r[6]    ));
	m[4] += (mul32x32_64(D(9), r[5] * 2) + mul32x32_64(D(8), r[6] * 2) + mul32x32_64(D(7), r[7]    ));
	m[5] += (mul32x32_64(D(9), r[6]    ) + mul32x32_64(D(8), r[7] * 2));
	m[6] += (mul32x32_64(D(9), r[7] * 2) + mul32x32_64(D(8), r[8]    ));
	m[7] += (mul32x32_64(D(9), r[8]    ));
	m[8] += (mul32x32_64(D(9), r[9]    ));

	                                 r[0] = (uint32_t)m[0] & reduce_mask_26; c = (m[0] >> 26);
	m[1] += c;                       r[1] = (uint32_t)m[1] & reduce_mask_25; c = (m[1] >> 25);
	m[2] += c;                       r[2] = (uint32_t)m[2] & reduce_mask_26; c = (m[2] >> 26);
	m[3] += c;                       r[3] = (uint32_t)m[3] & reduce_mask_25; c = (m[3] >> 25);
	m[4] += c;                       r[4] = (uint32_t)m[4] & reduce_mask_26; c = (m[4] >> 26);
	m[5] += c;                       r[5] = (uint32_t)m[5] & reduce_mask_25; c = (m[5] >> 25);
	m[6] += c;                       r[6] = (uint32_t)m[6] & reduce_mask_26; c = (m[6] >> 26);
	m[7] += c;                       r[7] = (uint32_t)m[7] & reduce_mask_25; c = (m[7] >> 25);
	m[8] += c;                       r[8] = (uint32_t)m[8] & reduce_mask_26; c = (m[8] >> 26);
	m[9] += c;                       r[9] = (uint32_t)m[9] & reduce_mask_25; p = (uint32_t)(m[9] >> 25);
	m[0] = r[0] + mul32x32_64(p,19); r[0] = (uint32_t)m[0] & reduce_mask_26; p = (uint32_t)(m[0] >> 26);
	r[1] += p;

	#undef D
}

/* out = in*in */
static void
curve25519_square(bignum25519 out, const bignum25519 in) {
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
	out[3] = in[3];
	out[4] = in[4];
	out[5] = in[5];
	out[6] = in[6];
	out[7] = in[7];
	out[8] = in[8];
	out[9] = in[9];

	curve25519_square_in_place( out );
}


/* out = in ^ (2 * count) */
static void
curve25519_square_times(bignum25519 out, const bignum25519 in, int count) {
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
	out[3] = in[3];
	out[4] = in[4];
	out[5] = in[5];
	out[6] = in[6];
	out[7] = in[7];
	out[8] = in[8];
	out[9] = in[9];

	do {
		curve25519_square_in_place( out );
	} while (--count);
}

/* Take a little-endian, 32-byte number and expand it into polynomial form */
static void
curve25519_expand(bignum25519 out, const unsigned char in[32]) {
#if defined( __MC68K__ )  ||  defined( __POWERPC__ )
	const uint8_t le = 0;
#else
	const uint16_t s = 1;
	const uint8_t le = *(const uint8_t*) &s;
#endif
	uint32_t x0,x1,x2,x3,x4,x5,x6,x7;

	if (le) {
		x0 = *(uint32_t *)(in + 0);
		x1 = *(uint32_t *)(in + 4);
		x2 = *(uint32_t *)(in + 8);
		x3 = *(uint32_t *)(in + 12);
		x4 = *(uint32_t *)(in + 16);
		x5 = *(uint32_t *)(in + 20);
		x6 = *(uint32_t *)(in + 24);
		x7 = *(uint32_t *)(in + 28);
    } else {
		#define F(s)                         \
			((((uint32_t)in[s + 0])      ) | \
			 (((uint32_t)in[s + 1]) <<  8) | \
			 (((uint32_t)in[s + 2]) << 16) | \
			 (((uint32_t)in[s + 3]) << 24))
		x0 = F(0);
		x1 = F(4);
		x2 = F(8);
		x3 = F(12);
		x4 = F(16);
		x5 = F(20);
		x6 = F(24);
		x7 = F(28);
		#undef F
	}

	out[0] = (                   x0       ) & 0x3ffffff;
	out[1] = ((x1 << 32 - 26) | (x0 >> 26)) & 0x1ffffff;
	out[2] = ((x2 << 32 - 19) | (x1 >> 19)) & 0x3ffffff;
	out[3] = ((x3 << 32 - 13) | (x2 >> 13)) & 0x1ffffff;
	out[4] = (                  (x3 >>  6)) & 0x3ffffff;
	out[5] = (                   x4       ) & 0x1ffffff;
	out[6] = ((x5 << 32 - 25) | (x4 >> 25)) & 0x3ffffff;
	out[7] = ((x6 << 32 - 19) | (x5 >> 19)) & 0x1ffffff;
	out[8] = ((x7 << 32 - 12) | (x6 >> 12)) & 0x3ffffff;
	out[9] = (                  (x7 >>  6)) & 0x1ffffff;
}

/* Take a fully reduced polynomial form number and contract it into a
 * little-endian, 32-byte array
 */
static void
curve25519_contract(unsigned char out[32], const bignum25519 in) {
	bignum25519 f;
	curve25519_copy(f, in);

	#define carry_pass() \
		f[1] += f[0] >> 26; f[0] &= reduce_mask_26; \
		f[2] += f[1] >> 25; f[1] &= reduce_mask_25; \
		f[3] += f[2] >> 26; f[2] &= reduce_mask_26; \
		f[4] += f[3] >> 25; f[3] &= reduce_mask_25; \
		f[5] += f[4] >> 26; f[4] &= reduce_mask_26; \
		f[6] += f[5] >> 25; f[5] &= reduce_mask_25; \
		f[7] += f[6] >> 26; f[6] &= reduce_mask_26; \
		f[8] += f[7] >> 25; f[7] &= reduce_mask_25; \
		f[9] += f[8] >> 26; f[8] &= reduce_mask_26;

	#define carry_pass_full() \
		carry_pass() \
		f[0] += 19 * (f[9] >> 25); f[9] &= reduce_mask_25;

	#define carry_pass_final() \
		carry_pass() \
		f[9] &= reduce_mask_25;

	carry_pass_full()
	carry_pass_full()

	/* now t is between 0 and 2^255-1, properly carried. */
	/* case 1: between 0 and 2^255-20. case 2: between 2^255-19 and 2^255-1. */
	f[0] += 19;
	carry_pass_full()

	/* now between 19 and 2^255-1 in both cases, and offset by 19. */
	f[0] += (reduce_mask_26 + 1) - 19;
	f[1] += (reduce_mask_25 + 1) - 1;
	f[2] += (reduce_mask_26 + 1) - 1;
	f[3] += (reduce_mask_25 + 1) - 1;
	f[4] += (reduce_mask_26 + 1) - 1;
	f[5] += (reduce_mask_25 + 1) - 1;
	f[6] += (reduce_mask_26 + 1) - 1;
	f[7] += (reduce_mask_25 + 1) - 1;
	f[8] += (reduce_mask_26 + 1) - 1;
	f[9] += (reduce_mask_25 + 1) - 1;

	/* now between 2^255 and 2^256-20, and offset by 2^255. */
	carry_pass_final()

	#undef carry_pass
	#undef carry_full
	#undef carry_final

	f[1] <<= 2;
	f[2] <<= 3;
	f[3] <<= 5;
	f[4] <<= 6;
	f[6] <<= 1;
	f[7] <<= 3;
	f[8] <<= 4;
	f[9] <<= 6;

	#define F(i, s) \
		out[s+0] |= (unsigned char )(f[i] & 0xff); \
		out[s+1] = (unsigned char )((f[i] >> 8) & 0xff); \
		out[s+2] = (unsigned char )((f[i] >> 16) & 0xff); \
		out[s+3] = (unsigned char )((f[i] >> 24) & 0xff);

	out[0] = 0;
	out[16] = 0;
	F(0,0);
	F(1,3);
	F(2,6);
	F(3,9);
	F(4,12);
	F(5,16);
	F(6,19);
	F(7,22);
	F(8,25);
	F(9,28);
	#undef F
}


/* out = (flag) ? in : out */
static void
curve25519_move_conditional_bytes(uint8_t out[96], const uint8_t in[96], uint32_t flag) {
	const uint32_t nb = flag - 1, b = ~nb;
	const uint32_t *inl = (const uint32_t *)in;
	uint32_t *outl = (uint32_t *)out;
	outl[0] = (outl[0] & nb) | (inl[0] & b);
	outl[1] = (outl[1] & nb) | (inl[1] & b);
	outl[2] = (outl[2] & nb) | (inl[2] & b);
	outl[3] = (outl[3] & nb) | (inl[3] & b);
	outl[4] = (outl[4] & nb) | (inl[4] & b);
	outl[5] = (outl[5] & nb) | (inl[5] & b);
	outl[6] = (outl[6] & nb) | (inl[6] & b);
	outl[7] = (outl[7] & nb) | (inl[7] & b);
	outl[8] = (outl[8] & nb) | (inl[8] & b);
	outl[9] = (outl[9] & nb) | (inl[9] & b);
	outl[10] = (outl[10] & nb) | (inl[10] & b);
	outl[11] = (outl[11] & nb) | (inl[11] & b);
	outl[12] = (outl[12] & nb) | (inl[12] & b);
	outl[13] = (outl[13] & nb) | (inl[13] & b);
	outl[14] = (outl[14] & nb) | (inl[14] & b);
	outl[15] = (outl[15] & nb) | (inl[15] & b);
	outl[16] = (outl[16] & nb) | (inl[16] & b);
	outl[17] = (outl[17] & nb) | (inl[17] & b);
	outl[18] = (outl[18] & nb) | (inl[18] & b);
	outl[19] = (outl[19] & nb) | (inl[19] & b);
	outl[20] = (outl[20] & nb) | (inl[20] & b);
	outl[21] = (outl[21] & nb) | (inl[21] & b);
	outl[22] = (outl[22] & nb) | (inl[22] & b);
	outl[23] = (outl[23] & nb) | (inl[23] & b);

}

/* if (iswap) swap(a, b) */
static void
curve25519_swap_conditional(bignum25519 a, bignum25519 b, uint32_t iswap) {
	const uint32_t swap = (uint32_t)(-(int32_t)iswap);
	uint32_t x0,x1,x2,x3,x4,x5,x6,x7,x8,x9;

	x0 = swap & (a[0] ^ b[0]); a[0] ^= x0; b[0] ^= x0;
	x1 = swap & (a[1] ^ b[1]); a[1] ^= x1; b[1] ^= x1;
	x2 = swap & (a[2] ^ b[2]); a[2] ^= x2; b[2] ^= x2;
	x3 = swap & (a[3] ^ b[3]); a[3] ^= x3; b[3] ^= x3;
	x4 = swap & (a[4] ^ b[4]); a[4] ^= x4; b[4] ^= x4;
	x5 = swap & (a[5] ^ b[5]); a[5] ^= x5; b[5] ^= x5;
	x6 = swap & (a[6] ^ b[6]); a[6] ^= x6; b[6] ^= x6;
	x7 = swap & (a[7] ^ b[7]); a[7] ^= x7; b[7] ^= x7;
	x8 = swap & (a[8] ^ b[8]); a[8] ^= x8; b[8] ^= x8;
	x9 = swap & (a[9] ^ b[9]); a[9] ^= x9; b[9] ^= x9;
}
