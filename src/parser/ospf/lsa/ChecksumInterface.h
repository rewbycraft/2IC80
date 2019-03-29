//
// Created by kaj on 27-3-19.
//

#ifndef ATTACK_CHECKSUMINTERFACE_H
#define ATTACK_CHECKSUMINTERFACE_H

#include <vector>


namespace parser {
    class ChecksumInterface {
    public:
        virtual std::vector<std::size_t> getEmptyByteIndices();
    };
}


#endif //ATTACK_CHECKSUMINTERFACE_H
