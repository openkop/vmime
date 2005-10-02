//
// VMime library (http://www.vmime.org)
// Copyright (C) 2002-2005 Vincent Richard <vincent@vincent-richard.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Linking this library statically or dynamically with other modules is making
// a combined work based on this library.  Thus, the terms and conditions of
// the GNU General Public License cover the whole combination.
//

#ifndef VMIME_NET_POP3_POP3STORE_HPP_INCLUDED
#define VMIME_NET_POP3_POP3STORE_HPP_INCLUDED


#include "vmime/config.hpp"

#include "vmime/net/store.hpp"
#include "vmime/net/socket.hpp"
#include "vmime/net/timeoutHandler.hpp"

#include "vmime/utility/stream.hpp"


namespace vmime {
namespace net {
namespace pop3 {


class POP3Folder;


/** POP3 store service.
  */

class POP3Store : public store
{
	friend class POP3Folder;
	friend class POP3Message;

public:

	POP3Store(ref <session> sess, ref <security::authenticator> auth);
	~POP3Store();

	const string getProtocolName() const;

	ref <folder> getDefaultFolder();
	ref <folder> getRootFolder();
	ref <folder> getFolder(const folder::path& path);

	const bool isValidFolderName(const folder::path::component& name) const;

	static const serviceInfos& getInfosInstance();
	const serviceInfos& getInfos() const;

	void connect();
	const bool isConnected() const;
	void disconnect();

	void noop();

	const int getCapabilities() const;

private:

	enum ResponseCode
	{
		RESPONSE_OK = 0,
		RESPONSE_READY,
		RESPONSE_ERR
	};

	void authenticate(const messageId& randomMID);
#if VMIME_HAVE_SASL_SUPPORT
	void authenticateSASL();
#endif // VMIME_HAVE_SASL_SUPPORT

	const std::vector <string> getCapabilities();

	static const bool isSuccessResponse(const string& buffer);
	static const bool stripFirstLine(const string& buffer, string& result, string* firstLine = NULL);
	static void stripResponseCode(const string& buffer, string& result);
	static const int getResponseCode(const string& buffer);

	void sendRequest(const string& buffer, const bool end = true);
	void readResponse(string& buffer, const bool multiLine, utility::progressionListener* progress = NULL);
	void readResponse(utility::outputStream& os, utility::progressionListener* progress = NULL, const int predictedSize = 0);

	static const bool checkTerminator(string& buffer, const bool multiLine);
	static const bool checkOneTerminator(string& buffer, const string& term);

	void internalDisconnect();


	void registerFolder(POP3Folder* folder);
	void unregisterFolder(POP3Folder* folder);

	std::list <POP3Folder*> m_folders;


	ref <socket> m_socket;
	bool m_authentified;

	ref <timeoutHandler> m_timeoutHandler;


	// Service infos
	class _infos : public serviceInfos
	{
	public:

		struct props
		{
			// POP3-specific options
			serviceInfos::property PROPERTY_OPTIONS_APOP;
			serviceInfos::property PROPERTY_OPTIONS_APOP_FALLBACK;
#if VMIME_HAVE_SASL_SUPPORT
			serviceInfos::property PROPERTY_OPTIONS_SASL;
			serviceInfos::property PROPERTY_OPTIONS_SASL_FALLBACK;
#endif // VMIME_HAVE_SASL_SUPPORT

			// Common properties
			serviceInfos::property PROPERTY_AUTH_USERNAME;
			serviceInfos::property PROPERTY_AUTH_PASSWORD;

			serviceInfos::property PROPERTY_SERVER_ADDRESS;
			serviceInfos::property PROPERTY_SERVER_PORT;
			serviceInfos::property PROPERTY_SERVER_SOCKETFACTORY;

			serviceInfos::property PROPERTY_TIMEOUT_FACTORY;
		};

		const props& getProperties() const;

		const string getPropertyPrefix() const;
		const std::vector <serviceInfos::property> getAvailableProperties() const;
	};

	static _infos sm_infos;
};


} // pop3
} // net
} // vmime


#endif // VMIME_NET_POP3_POP3STORE_HPP_INCLUDED