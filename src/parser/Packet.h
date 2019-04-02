//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_PACKET_H
#define ATTACK_PACKET_H

#include <vector>
#include <cstdint>
#include "internal.h"
#include <functional>
#include <string>

namespace parser {
	class Packet {
	public:
		/**
		 * Empty constructor.
		 * Creates a new packet with the default values.
		 */
		Packet();
		
		/**
		 * Deserialization constructor.
		 * Creates a packet from a bytevector.
		 */
		explicit Packet(const bytevector& data);

		/**
		 * Deconstructor.
		 */
		virtual ~Packet();

		/**
		 * Serializes this packet.
		 * The returned vector is in big-endian notation and can be used to directly
		 * send it on a network.
		 *
		 * @return a bytevector representing the serialized vector of this packet.
		 */
		virtual const bytevector serialize() const = 0;

		/**
		 *
		 * @param printer
		 */
		virtual void toString(const std::function<void(const std::string&)>& printer) const = 0;

		/**
		 * Wrapper toString function of the {@link #toString(const std::function)}.
		 *
		 * @return a string representation of the packet.
		 */
		virtual const std::string toString() const;

		/**
		 * Updates all values which needs updating, e.g. packet length.
		 * The packets should be updated using depth iteration.
		 */
		virtual void updateValues() = 0;
	};
}

#endif //ATTACK_PACKET_H
