/**
 * The TibiaX Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2014  TibiaX <http://tibiax.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FS_PROTOCOL_H_D71405071ACF4137A4B1203899DE80E1
#define FS_PROTOCOL_H_D71405071ACF4137A4B1203899DE80E1

class NetworkMessage;
class OutputMessage;
class Connection;
typedef std::shared_ptr<OutputMessage> OutputMessage_ptr;
typedef std::shared_ptr<Connection> Connection_ptr;
class RSA;

class Protocol
{
	public:
		Protocol(Connection_ptr connection) : m_connection(connection), m_key(), m_refCount(), m_encryptionEnabled(false), m_checksumEnabled(true), m_rawMessages(false) {}

		// non-copyable
		Protocol(const Protocol&) = delete;
		Protocol& operator=(const Protocol&) = delete;

		virtual ~Protocol() {}

		virtual int32_t getProtocolId() {
			return 0x00;
		}

		virtual void parsePacket(NetworkMessage& msg) {}

		virtual void onSendMessage(OutputMessage_ptr msg);
		void onRecvMessage(NetworkMessage& msg);
		virtual void onRecvFirstMessage(NetworkMessage& msg) = 0;
		virtual void onConnect() {}

		Connection_ptr getConnection() const {
			return m_connection;
		}
		void setConnection(Connection_ptr connection) {
			m_connection = connection;
		}

		uint32_t getIP() const;

		int32_t addRef() {
			return ++m_refCount;
		}
		int32_t unRef() {
			return --m_refCount;
		}

		//Use this function for autosend messages only
		OutputMessage_ptr getOutputBuffer(int32_t size);

	protected:
		void enableXTEAEncryption() {
			m_encryptionEnabled = true;
		}
		void disableXTEAEncryption() {
			m_encryptionEnabled = false;
		}
		void setXTEAKey(const uint32_t* key) {
			memcpy(m_key, key, sizeof(uint32_t) * 4);
		}
		void enableChecksum() {
			m_checksumEnabled = true;
		}
		void disableChecksum() {
			m_checksumEnabled = false;
		}

		void XTEA_encrypt(OutputMessage& msg) const;
		bool XTEA_decrypt(NetworkMessage& msg) const;
		bool RSA_decrypt(NetworkMessage& msg);

		void setRawMessages(bool value) {
			m_rawMessages = value;
		}

		virtual void releaseProtocol();
		virtual void deleteProtocolTask();
		friend class Connection;

		OutputMessage_ptr m_outputBuffer;

	private:
		Connection_ptr m_connection;
		uint32_t m_key[4];
		uint32_t m_refCount;
		bool m_encryptionEnabled;
		bool m_checksumEnabled;
		bool m_rawMessages;
};

#endif
