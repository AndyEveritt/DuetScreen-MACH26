/*
 * BoardSubscribers.h
 *
 *  Created on: 2025-12-01
 *      Author: Andy Everitt
 */

#pragma once

#include "Subscribers/Subscribers.h"

class BoardSubscribers : public SubscriberMap
{
  public:
	BoardSubscribers()
	{
		addSubscriber("boards^:uniqueId", uniqueId);
	}

  private:
	static bool uniqueId(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
};