//
// Created by kaj on 27-3-19.
//

#ifndef ATTACK_CHECKSUMINTERFACE_H
#define ATTACK_CHECKSUMINTERFACE_H

#include <vector>


namespace parser {
    class ChecksumInterface {
    public:
        /**
         * @return a vector containing the indices of the bytes
         *     in this packet which are allowed to be modified.
         */
        virtual std::vector<std::size_t> getEmptyByteIndices();
    };
}


#endif //ATTACK_CHECKSUMINTERFACE_H
