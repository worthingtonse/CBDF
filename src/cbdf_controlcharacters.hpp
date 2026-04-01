#include <cstdint>

enum class ControlChar : std::uint32_t {
    FS = 0x1C, // File Separator
    GS = 0x1D, // Group Separator
    RS = 0x1E, // Record Separator
    US = 0x1F  // Unit Separator
};