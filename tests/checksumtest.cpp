//
// Created by kaj on 27-3-19.
//

#include "gtest/gtest.h"
#include <parser/internal.h>
#include <parser/ospf/OSPFv3Packet.h>
#include <parser/ospf/lsa/LSAPacket.h>
#include <parser/ospf/lsa/IntraAreaPrefixLSAPacket.h>
#include <parser/ospf/lsa/PrefixLSAPacket.h>
#include <parser/ospf/LinkStateUpdatePacket.h>
#include <parser/ospf/lsa/RouterLSAPacket.h>


TEST(checksumtest, recalcChecksumTest01) {
    parser::bytevector data = {0x03, 0x04, 0x00, 0x6c, 0x01, 0x01, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x67, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x02, 0x80, 0x00, 0x00, 0x07, 0x4c, 0x01, 0x00, 0x58, 0x02, 0x00, 0x01, 0x13, 0x01, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x16, 0x01, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x1c, 0x01, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x1a, 0x01, 0x01, 0x02, 0x02, 0x01, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x28, 0x01, 0x01, 0x06, 0x01};
    parser::OSPFv3Packet packet(data);
    packet.setSource(std::make_uint128(18338657682652659712u,9252625643192011738u));
    packet.setDest(std::make_uint128(18338657682652659712u,5510013406568449447u));
    packet.updateValues();

    std::optional<std::shared_ptr<parser::LSAPacket>> newPacket = std::nullopt;
    std::shared_ptr<parser::LinkStateUpdatePacket> lsu = std::dynamic_pointer_cast<parser::LinkStateUpdatePacket>(
            packet.getSubpacket());
    std::vector<std::shared_ptr<parser::LSAPacket>> lsas = lsu-> getLsas();
    std::shared_ptr<parser::LSAPacket> lsa = lsas[0];
    const uint16_t oldChecksum = lsa->getHeader().checksum;
    std::shared_ptr<parser::RouterLSAPacket> rLSAp = std::dynamic_pointer_cast<parser::RouterLSAPacket>(
            lsa->getSubpacket());
    std::vector<parser::RouterLSAPacket::InterfaceHeader> hdrs = rLSAp->getInterfaces();
    const parser::RouterLSAPacket::InterfaceHeader &origHeader = hdrs[0];

    const std::uint16_t targetChecksum = 1000; // oldChecksum;
    for (std::uint16_t metric = 10000; !newPacket; metric++) {
        const parser::RouterLSAPacket::InterfaceHeader newHeader = {
                origHeader.type, origHeader.unused, uint16_t(metric),
                origHeader.interface_id, origHeader.neighbor_interface_id,
                origHeader.neighbor_router_id
        };
        hdrs[0] = newHeader;
        rLSAp->setInterfaces(hdrs);
        newPacket = lsa->modToChecksum(targetChecksum);
    }

    lsas[0] = newPacket.value();
    lsas[0]->updateValues();
    lsu->setLsas(lsas);
    packet.updateValues();
    const std::uint16_t newChecksum = newPacket.value()->getHeader().checksum;

    EXPECT_EQ(targetChecksum, newChecksum);
}


