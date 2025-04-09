/*
 * DirectoriesSubscribers.h
 *
 *  Created on: 2025-04-09
 *      Author: Andy Everitt
 */

#pragma once

#include "Subscribers/Subscribers.h"

class DirectoriesSubscribers : public SubscriberMap
{
  public:
	DirectoriesSubscribers()
	{
		addSubscriber("directories:filaments", filaments);
		addSubscriber("directories:firmware", firmware);
		addSubscriber("directories:gcodes", gcodes);
		addSubscriber("directories:macros", macros);
		addSubscriber("directories:menu", menu);
		addSubscriber("directories:system", system);
		addSubscriber("directories:web", web);
	}

  private:
	static bool filaments(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool firmware(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool gcodes(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool macros(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool menu(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool system(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool web(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
};
