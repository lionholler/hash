#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


const uint32_t K[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
			0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
			0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
			0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
			0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
			0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
			0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
			0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

struct grid {
	uint32_t a; // 0x6a09e667; 
	uint32_t b; // 0xbb67ae85; 
	uint32_t c; // 0x3c6ef372; 
	uint32_t d; // 0xa54ff53a; 
	uint32_t e; // 0x510e527f; 
	uint32_t f; // 0x9b05688c; 
	uint32_t g; // 0x1f83d9ab; 
	uint32_t h; // 0x5be0cd19;
};

static void printh(struct grid *g) {
	printf("%08" PRIx32, g->a);
	printf("%08" PRIx32, g->b);
	printf("%08" PRIx32, g->c);
	printf("%08" PRIx32, g->d);
	printf("%08" PRIx32, g->e);
	printf("%08" PRIx32, g->f);
	printf("%08" PRIx32, g->g);
	printf("%08" PRIx32, g->h);
	printf("\n");
}

uint32_t u8tou32(uint8_t *b8, int s) {
	uint32_t u32t = b8[s];
	for (int i = 1; i < 4; i++) {
		u32t = (u32t << 8) + b8[s+i];
	}
	return u32t;
}

uint32_t rightrotn32(uint32_t u32, int n) {
	return (u32 >> n) | (u32 << (32 - n));
}

uint32_t sigma0(uint32_t u32) {
	return rightrotn32(u32, 7) ^ rightrotn32(u32, 18) ^ (u32 >> 3);
}

uint32_t sigma1(uint32_t u32) {
	return rightrotn32(u32, 17) ^ rightrotn32(u32, 19) ^ (u32 >> 10);
}

uint32_t Sigma0(uint32_t u32) {
	return rightrotn32(u32, 2) ^ rightrotn32(u32, 13) ^ rightrotn32(u32, 22);
}

uint32_t Sigma1(uint32_t u32) {
	return rightrotn32(u32, 6) ^ rightrotn32(u32, 11) ^ rightrotn32(u32, 25);
}

uint32_t choice(uint32_t x, uint32_t y, uint32_t z) {
	uint32_t u32 = 0;
	for (int i = 31; i >= 0; i--) {
		if ((x >> i) & 0x1) {
			u32 = (u32 << 1) | (((y >> i) & 0x1) ? 0x1 : 0x0); 
		} else {
			u32 = (u32 << 1) | (((z >> i) & 0x1) ? 0x1 : 0x0);
		}
	}
	return u32;
}

uint32_t majority(uint32_t x, uint32_t y, uint32_t z) {
	uint32_t u32 = 0;
	for (int i = 31; i >= 0; i--) {
		if ((((x >> i) & 0x1) && ((y >> i) & 0x1)) || (((x >> i) & 0x1) && ((z >> i) & 0x1)) || (((y >> i) & 0x1) && ((z >> i) & 0x1))) { 
			u32 = (u32 << 1) | 0x1;
		} else {
			u32 = (u32 << 1);
		}
	}
	return u32;
}

void init_grid(struct grid *grid) {
	grid->a = 0x6a09e667; 
	grid->b = 0xbb67ae85; 
	grid->c = 0x3c6ef372; 
	grid->d = 0xa54ff53a; 
	grid->e = 0x510e527f; 
	grid->f = 0x9b05688c; 
	grid->g = 0x1f83d9ab; 
	grid->h = 0x5be0cd19;
}

void grid_operation(struct grid *grid, uint32_t T1, uint32_t T2) {
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
// SHA-256 takes in blocks of 512 bits. Max message length equals 2**64.
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

	uint8_t words[64];
	struct grid g, hashvalues;
	init_grid(&g);

	int nblocks = ((l + 72) + 504) / 512;
	if (verbose) 
		printf("number of rounds/blocks: %d\n\n", nblocks);

	short set_padd_seperator = 0;

	uint32_t message_schedule_32[64];
	uint32_t T1, T2;

	for (int r = 0; r < nblocks; r++) {
		short i;
		cpygrid(&hashvalues, &g); 

		for (i = 0; i < 64; i++) {
			words[i] = 0x00;
		}

		if (r == nblocks-1) { // last round
			while (i > 55) {
				words[--i] = (uint8_t) (l & 0xFF);
				l >>= 8;
			}

			for (i = 0; *s != '\0' && i < 56; s++) { // 56 should never be reached
				words[i++] = (uint8_t) *s;
			}
			if (*s == '\0' && !set_padd_seperator) {
				words[i] = 0x80; 
			}

		} else {
			for (i = 0; *s != '\0' && i < 64; s++) {
				words[i++] = (uint8_t) *s;
			}
			if (*s == '\0' && i < 64) {
				words[i] = 0x80; 
				set_padd_seperator = 1;
			}
		}

		for (i = 0; i < 16; i++) {
			message_schedule_32[i] = u8tou32(words, i*4);
		}

		for (; i < 64; i++) {
			message_schedule_32[i] = message_schedule_32[i - 16] 
						+ sigma0(message_schedule_32[i - 15]) 
						+ message_schedule_32[i - 7] 
						+ sigma1(message_schedule_32[i - 2]);
		}

		for (i = 0; i < 64; i++) { // compression
			T1 = Sigma1(g.e) + choice(g.e, g.f, g.g) + g.h + K[i] + message_schedule_32[i];
			T2 = Sigma0(g.a) + majority(g.a, g.b, g.c);
			grid_operation(&g, T1, T2);
		}

		grid_addition_k(&g, &hashvalues); 
	}

	printh(&g);
}
