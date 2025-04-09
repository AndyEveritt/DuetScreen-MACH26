/*
 * {{file_name_with_ext}}
 *
 *  Created on: {{date}}
 *      Author: {{author}}
 */

#pragma once

#include "Subscribers/Subscribers.h"

class {{file_name}} : public SubscriberMap
{
  public:
	{{file_name}}()
	{
		addSubscriber("key", callback);
		addArrayEndSubscriber("key^", arrayEnd);
	}

  private:
	static bool callback(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool arrayEnd(Comm::JsonDecoder* decoder, const size_t indices[]);
};
