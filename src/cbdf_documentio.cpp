#include "cbdf_documentio.hpp"

#include <meta>
#include <iostream>

void writeBinaryData(std::ostream& os, std::span<const std::byte> data) {
    os.write(reinterpret_cast<const char*>(data.data()), data.size_bytes());
}

// Archive for pushing out bytes and reading in bytes
class Archive {
public:
    void read() {

    }

    /*
    write will take x (somehow the objects that we fetch via reflection) 
    and turn them into binary, then checking that they are LE (check utils)
    then write them to the file. 

    
    */
    void write() {

    }
};

