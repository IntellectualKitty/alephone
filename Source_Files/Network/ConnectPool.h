#ifndef __CONNECTPOOL_H
#define __CONNECTPOOL_H

/*

	Copyright (C) 2007 Gregory Smith.
	and the "Aleph One" developers.
 
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	This license is contained in the file "COPYING",
	which is included with this source code; it is available online at
	http://www.gnu.org/licenses/gpl.html

	A pool for non-blocking outbound TCP connections

*/

#include "cseries.h"
#include "CommunicationsChannel.h"
#include <string>
#include <memory>
#include <SDL/SDL_thread.h>

class NonblockingConnect
{
public:
	NonblockingConnect(const std::string& address, uint16 port);
	NonblockingConnect(const IPaddress& ip);
	~NonblockingConnect();

	enum Status
	{
		Connecting,
		Connected,
		ResolutionFailed,
		ConnectFailed
	};

	Status status() { return m_status; }
	bool done() { return m_status != Connecting; }
	const IPaddress& address() { 
		assert(m_status != Connecting && m_status != ResolutionFailed); 
		return m_ip;
	}
	
	CommunicationsChannel* release() { 
		assert(m_status == Connected); 
		return m_channel.release();
	}


private:
	void connect();
	std::auto_ptr<CommunicationsChannel> m_channel;
	Status m_status;

	std::string m_address;
	uint16 m_port;

	bool m_ipSpecified;
	IPaddress m_ip;

	int Thread();
	static int connect_thread(void *);
	SDL_Thread *m_thread;
};	
		

class ConnectPool
{
public:
	static ConnectPool *instance() { if (!m_instance) m_instance = new ConnectPool(); return m_instance; }
	NonblockingConnect* connect(const std::string& address, uint16 port);
	NonblockingConnect* connect(const IPaddress& ip);
	void abandon(NonblockingConnect*);
	~ConnectPool();

private:
	ConnectPool();
	void fast_free();
	enum { kPoolSize = 20 };
	// second is false if we are in use!
	std::pair<NonblockingConnect *, bool> m_pool[kPoolSize];

	static ConnectPool* m_instance;
};

#endif
