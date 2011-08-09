#include "sys.h"

typedef
enum Rkind {
	/* type representations */
	Rundef,
	Ru08le,
	Ru16le,
	Ru32le,
	Ru64le,
	Rs08le,
	Rs16le,
	Rs32le,
	Rs64le,
	Ru08be,
	Ru16be,
	Ru32be,
	Ru64be,
	Rs08be,
	Rs16be,
	Rs32be,
	Rs64be,
	Rnirep,
	Rf32=Rnirep,
	Rf64,
	Rnrep,
} Rkind;

static char* repname[Rnrep] = {
	[Ru08le]=	"Ru08le",
	[Ru16le]=	"Ru16le",
	[Ru32le]=	"Ru32le",
	[Ru64le]=	"Ru64le",
	[Rs08le]=	"Rs08le",
	[Rs16le]=	"Rs16le",
	[Rs32le]=	"Rs32le",
	[Rs64le]=	"Rs64le",
	[Ru08be]=	"Ru08be",
	[Ru16be]=	"Ru16be",
	[Ru32be]=	"Ru32be",
	[Ru64be]=	"Ru64be",
	[Rs08be]=	"Rs08be",
	[Rs16be]=	"Rs16be",
	[Rs32be]=	"Rs32be",
	[Rs64be]=	"Rs64be",
	[Rf32]=		"Rf32",
	[Rf64]=		"Rf64",
};

static char* reptype[Rnrep] = {
	[Ru08le]=	"u8",
	[Ru16le]=	"u16",
	[Ru32le]=	"u32",
	[Ru64le]=	"u64",
	[Rs08le]=	"s8",
	[Rs16le]=	"s16",
	[Rs32le]=	"s32",
	[Rs64le]=	"s64",
	[Ru08be]=	"u8",
	[Ru16be]=	"u16",
	[Ru32be]=	"u32",
	[Ru64be]=	"u64",
	[Rs08be]=	"s8",
	[Rs16be]=	"s16",
	[Rs32be]=	"s32",
	[Rs64be]=	"s64",
	[Rf32]=		"f32",
	[Rf64]=		"f64",
};

static char* fpvar[Rnrep] = {
	[Rf32]=		"f",
	[Rf64]=		"d",
};

int
main(int argc, char *argv[])
{
	unsigned i, j;

	/* integer -> integer */
	for(i = 1; i < Rnirep; i++)
		for(j = 1; j < Rnirep; j++){
			printf("case (%s<<5)|%s:\n",
			       repname[i], repname[j]);
			printf("\tv.u = (%s)(%s)v.u;\n",
			       reptype[i], reptype[j]);
			printf("\tbreak;\n");
		}

	/* integer -> float */
	for(i = Rf32; i <= Rf64; i++)
		for(j = 1; j < Rnirep; j++){
			printf("case (%s<<5)|%s:\n",
			       repname[i], repname[j]);
			printf("\tv.%s = (%s)(%s)v.u;\n",
			       fpvar[i], reptype[i], reptype[j]);
			printf("\tbreak;\n");
		}

	/* float -> integer */
	for(i = 1; i < Rnirep; i++)
		for(j = Rf32; j <= Rf64; j++){
			printf("case (%s<<5)|%s:\n",
			       repname[i], repname[j]);
			printf("\tv.u = (%s)v.%s;\n",
			       reptype[i], fpvar[j]);
			printf("\tbreak;\n");
		}

	/* float -> float */
	for(i = Rf32; i <= Rf64; i++)
		for(j = Rf32; j <= Rf64; j++){
			printf("case (%s<<5)|%s:\n",
			       repname[i], repname[j]);
			printf("\tv.%s = v.%s;\n",
			       fpvar[i], fpvar[j]);
			printf("\tbreak;\n");
		}
}
