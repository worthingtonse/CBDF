/*
Authors: Victoria Worthington & Ryan Makela 
Version: 0.0.1
*/
#include <map>
#include <cstdint>
#include <unordered_map>
#include <span>
#include <string>
#include <concepts>
#include <inplace_vector>

#include "cbdf_controlcharacters.hpp"

template<uint8_t MetadataKey, 
	typename MetadataType,
	bool MetadataRequired>
struct MetadataField {
	static constexpr uint8_t key = MetadataKey;
	using type = MetadataType;
	static constexpr bool required = MetadataRequired;
};

struct QMailId {
	std::array<std::byte, 16> GUID;
};

struct MailboxAddress {
	std::uint16_t coin_group;
	std::uint8_t demonination;
	std::uint32_t serial_number;
};

using QMailSchema = std::tuple<
	MetadataField<1, QMailId, true>,		// QMail ID
	MetadataField<2, std::string, false>,	// Subject
	MetadataField<12, uint8_t, true>,		// Attachment Count
	MetadataField<13, MailboxAddress, true>,  // To Mailbox
	MetadataField<14, MailboxAddress, false>, // CC Mailbox
	MetadataField<19, MailboxAddress, true>,  // From Mailbox
	MetadataField<25, uint32_t, true>		// Timestamp
>;

template <std::unsigned_integral keySize,
	std::unsigned_integral valueSize>
struct TLV {
	keySize key_id;
	valueSize value_length;
	std::inplace_vector<std::byte, 255> value; // max size of 255 bytes. 
};

template <typename Schema, typename Func>
void dispatch_tlv(const TLV<uint8_t, uint8_t>& tlv, Func&& callback) {
	bool found = false;

	// C++26 template for iterates over the tuple types at compile time
	template for (using Field : Schema) {
		if (tlv.key_id == Field::key) {
			// We found the matching schema entry!
			using TargetType = typename Field::type;

			// Map the raw bytes to our actual struct
			TargetType value;
			std::memcpy(&value, tlv.value.data(), sizeof(TargetType));

			callback(value);
			found = true;
		}
	}

	if (!found) {
		// Handle unknown Key ID
	}
}

class Metadata {
	std::uint16_t pair_count;
	std::inplace_vector<TLV<uint8_t, uint8_t>, pair_count> tlvs;
	
public:
	Metadata(std::uint8_t pair_count) : pair_count(pair_count) {
		
	}
};

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