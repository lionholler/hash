#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

const uint64_t K[128] ={0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
			0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
			0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
			0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
			0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
			0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
			0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
			0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
			0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
			0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
			0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
			0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
			0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
			0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
			0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
			0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
			0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
			0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
			0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
			0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817};

struct grid {
	uint64_t a; // 0x6a09e667f3bcc908; 
	uint64_t b; // 0xbb67ae8584caa73b; 
	uint64_t c; // 0x3c6ef372fe94f82b; 
	uint64_t d; // 0xa54ff53a5f1d36f1; 
	uint64_t e; // 0x510e527fade682d1; 
	uint64_t f; // 0x9b05688c2b3e6c1f; 
	uint64_t g; // 0x1f83d9abfb41bd6b; 
	uint64_t h; // 0x5be0cd19137e2179;
};

static void printh(struct grid *g) {
	printf("%08" PRIx64, g->a);
	printf("%08" PRIx64, g->b);
	printf("%08" PRIx64, g->c);
	printf("%08" PRIx64, g->d);
	printf("%08" PRIx64, g->e);
	printf("%08" PRIx64, g->f);
	printf("%08" PRIx64, g->g);
	printf("%08" PRIx64, g->h);
	printf("\n");
}

uint64_t u8tou64(uint8_t *b8, int s) {
	uint64_t u64t = b8[s];
	for (int i = 1; i < 8; i++) {
		u64t = (u64t << 8) + b8[s+i];
	}
	return u64t;
}

uint64_t rightrotn32(uint64_t u64, int n) {
	return (u64 >> n) | (u64 << (64 - n));
}

uint64_t sigma0(uint64_t u64) {
	return rightrotn32(u64, 1) ^ rightrotn32(u64, 8) ^ (u64 >> 7);
}

uint64_t sigma1(uint64_t u64) {
	return rightrotn32(u64, 19) ^ rightrotn32(u64, 61) ^ (u64 >> 6);
}

uint64_t Sigma0(uint64_t u64) {
	return rightrotn32(u64, 28) ^ rightrotn32(u64, 34) ^ rightrotn32(u64, 39);
}

uint64_t Sigma1(uint64_t u64) {
	return rightrotn32(u64, 14) ^ rightrotn32(u64, 18) ^ rightrotn32(u64, 41);
}

uint64_t choice(uint64_t x, uint64_t y, uint64_t z) {
	uint64_t u64 = 0;
	for (int i = 127; i >= 0; i--) {
		if ((x >> i) & 0x1) {
			u64 = (u64 << 1) | (((y >> i) & 0x1) ? 0x1 : 0x0); 
		} else {
			u64 = (u64 << 1) | (((z >> i) & 0x1) ? 0x1 : 0x0);
		}
	}
	return u64;
}

uint64_t majority(uint64_t x, uint64_t y, uint64_t z) {
	uint64_t u64 = 0;
	for (int i = 127; i >= 0; i--) {
		if ((((x >> i) & 0x1) && ((y >> i) & 0x1)) || (((x >> i) & 0x1) && ((z >> i) & 0x1)) || (((y >> i) & 0x1) && ((z >> i) & 0x1))) { 
			u64 = (u64 << 1) | 0x1;
		} else {
			u64 = (u64 << 1);
		}
	}
	return u64;
}

void init_grid(struct grid *grid) {
	grid->a = 0x6a09e667f3bcc908; 
	grid->b = 0xbb67ae8584caa73b; 
	grid->c = 0x3c6ef372fe94f82b; 
	grid->d = 0xa54ff53a5f1d36f1; 
	grid->e = 0x510e527fade682d1; 
	grid->f = 0x9b05688c2b3e6c1f; 
	grid->g = 0x1f83d9abfb41bd6b; 
	grid->h = 0x5be0cd19137e2179;
}

void grid_operation(struct grid *grid, uint64_t T1, uint64_t T2) {
	grid->h = grid->g;
	grid->g = grid->f;
	grid->f = grid->e;
	grid->e = grid->d + T1;
	grid->d = grid->c;
	grid->c = grid->b;
	grid->b = grid->a;
	grid->a = T1 + T2;
}

void grid_addition_k(struct grid *grid, struct grid *hashvalues) {
	grid->a += hashvalues->a;
	grid->b += hashvalues->b;
	grid->c += hashvalues->c;
	grid->d += hashvalues->d;
	grid->e += hashvalues->e;
	grid->f += hashvalues->f;
	grid->g += hashvalues->g;
	grid->h += hashvalues->h;
}

void cpygrid(struct grid *hashvalues, struct grid *grid) {
	hashvalues->a = grid->a;
	hashvalues->b = grid->b;
	hashvalues->c = grid->c;
	hashvalues->d = grid->d;
	hashvalues->e = grid->e;
	hashvalues->f = grid->f;
	hashvalues->g = grid->g;
	hashvalues->h = grid->h;
}

int main(int argc, char *argv[]) { 
// SHA-512 takes in blocks of 1024 bits. Max message length equals 2**128.
// | MESSAGE | PADDING | MESSAGE LENGTH |

	char *s;
	uint64_t l;
	short argv_index = 1, verbose = 0;

	if (*(argv[1]) == '-') { 
	// flag specified
		if ((argc < 3 || argc > 3) && *(argv[1]+1) != 'h') {
			printf("%s%s%s\n", "error: too ", (argc > 3) ? "many" : "few", " arguments");
			return -1;
		} 
		switch (*(++argv[1])) {
		// check which flag is specified
			case 'f': {
			// read string from a file, suffix \n is removed!
				if (access(argv[2], F_OK) != 0) {
					printf("%s\n", "error: specified file does not exist");
					return -1;
				}
				FILE *f = fopen(argv[2], "rb");
				fseek(f, 0, SEEK_END);
				l = ftell(f);
				fseek(f, 0, SEEK_SET); 
				if ((s = malloc(l + 1)) == NULL) {
					printf("%s\n", "error: malloc for file buffer failed");
					return -1;
				}
				if (fread(s, 1, l, f) < l) {
					printf("%s\n", "error: fread failed reading in all file content");
					return -1;
				}
				fclose(f);
				s[--l] = '\0'; // overwriting \n at end of string, might not want to do this :)
				l *= 8;
				break;
			} case 'v': {
				// slightly verbose, showing message len and some othe things
				++argv_index;
				verbose = 1;
				goto default_path;
			} case 'h': {
				printf("%s", "sha256\t-f\tloads in the content of the specified file name, trailing \\n is removed!\n\t-v\tgives a somwhat more verbose output\n\n\tformat: sha256 [flag] [string/file name]\n\n");
				return 0;
			} default:
				printf("%s\n", "error: unknown flag, -h to see all available flags");
				return -1;

		}
		++argv_index;
	} else { 
	// sha256 hash the provided argv1 argument
		if (argc > 2) {
			printf("%s\n", "error: too many arguments");
			return -1;
		}
		default_path: 
		// go here if we have the message as argument
			s = argv[argv_index];
			l = strlen(s) * 8;
			if (verbose)
				printf("\nSHA-256 hash verbose\ninput lenght: %llu bits\n", l);
	}

	uint8_t words[128];
	struct grid g, hashvalues;
	init_grid(&g);

	int nblocks = ((l + 1152) / 1024);
	if (verbose) 
		printf("number of rounds/blocks: %d\n\n", nblocks);

	short set_padd_seperator = 0;

	uint64_t message_schedule_64[80];
	uint64_t T1, T2;

	for (int r = 0; r < nblocks; r++) {
		short i;
		cpygrid(&hashvalues, &g); 

		for (i = 0; i < 128; i++) {
			words[i] = 0x00;
		}

		if (r == nblocks-1) { // last round
			while (i > 111) {
				words[--i] = (uint8_t) (l & 0xFF);
				l >>= 8;
			}

			for (i = 0; *s != '\0' && i < 112; s++) { // 112 should never be reached
				words[i++] = (uint8_t) *s;
			}
			if (*s == '\0' && !set_padd_seperator) {
				words[i] = 0x80; //0x1 << 7;
			}

		} else {
			for (i = 0; *s != '\0' && i < 128; s++) {
				words[i++] = (uint8_t) *s;
			}
			if (*s == '\0' && i < 128) {
				words[i] = 0x80; 
				set_padd_seperator = 1;
			}
		}
		
		for (i = 0; i < 16; i++) {
			message_schedule_64[i] = u8tou64(words, i*8);
		}

		for (; i < 80; i++) {
			message_schedule_64[i] = sigma1(message_schedule_64[i - 2])
								   + message_schedule_64[i - 7] 
								   + sigma0(message_schedule_64[i - 15]) 
								   + message_schedule_64[i - 16];
		}

		for (i = 0; i < 80; i++) { // compression
			T1 = Sigma1(g.e) + choice(g.e, g.f, g.g) + g.h + K[i] + message_schedule_64[i];
			T2 = Sigma0(g.a) + majority(g.a, g.b, g.c);
			grid_operation(&g, T1, T2);
		}

		grid_addition_k(&g, &hashvalues); 
	}

	printh(&g);
}
