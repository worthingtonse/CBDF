/*
Authors: Victoria Worthington & Ryan Makela 
Version: 0.0.1
*/

#include <map>
#include <cstdint>

struct Document;

struct Document {
	// investigate more into this pragma pack, pop, and static assert
	#pragma pack(push, 1) // Ensure 0 padding
	struct Metadata {
		std::uint16_t pair_count;
	};
	#pragma pack(pop)

	struct Styles {
		std::uint16_t pair_count;
	};

	struct Text {
		std::uint16_t pair_count;
	};

	struct Resources {
		std::uint16_t pair_count;
	};

	struct Logic {
		std::uint16_t pair_count;
	};


	Metadata meta; 
	Styles styles;
	Text text;
	Resources resources;
	Logic logic;
};