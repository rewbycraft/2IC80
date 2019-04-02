//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_MALFORMEDPACKETEXCEPTION_H
#define ATTACK_MALFORMEDPACKETEXCEPTION_H

#include <exception>
#include <string>


namespace parser {
	/**
	 * This exception should be thrown when a malformed packet
	 * was found during parsing.
	 */
	class MalformedPacketException : public std::exception {
		const std::string reason;
	public:
		MalformedPacketException(const std::string &reason);
		
		const char *what() const noexcept override;
	};
}

#endif //ATTACK_MALFORMEDPACKETEXCEPTION_H
