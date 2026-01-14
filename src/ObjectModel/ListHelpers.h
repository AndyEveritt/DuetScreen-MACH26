/*
 * ListHelpers.h
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#ifndef SRC_OBJECTMODEL_LISTHELPERS_HPP_
#define SRC_OBJECTMODEL_LISTHELPERS_HPP_

#include "Debug.h"

// #include <cstdint>
#include <Duet3D/General/function_ref.h>
#include <memory>
#include <sys/types.h>

template <typename L, typename T>
std::shared_ptr<T> GetOrCreate(L& list, const size_t index, const bool create, const bool silent = false)
{
	const size_t count = list.Size();
	for (size_t i = 0; i < count; ++i)
	{
		if (list[i]->index == index)
		{
			LOG_VERBOSE("Getting index={:d}", index);
			return list[i];
		}
	}

	if (create && !list.Full())
	{
		LOG_VERBOSE("Creating index={:d}", index);
		std::shared_ptr<T> elem = std::make_shared<T>();
		elem->Reset();
		elem->index = index;
		list.Add(elem);
		list.Sort([](std::shared_ptr<T> e1, std::shared_ptr<T> e2) { return e1->index > e2->index; });
		return elem;
	}

	if (!silent)
		LOG_ERROR("Failed to get{:s} index={:d}", create ? " or create" : "", index);
	return nullptr;
}

template <typename L, typename T>
std::shared_ptr<T> Find(L& list, function_ref<bool(std::shared_ptr<T>)> filter)
{
	const size_t count = list.Size();
	for (size_t i = 0; i < count; ++i)
	{
		if (filter(list[i]))
		{
			return list[i];
		}
	}
	return nullptr;
}

template <typename L, typename T>
size_t Remove(L& list, const size_t index, const bool allFollowing)
{
	// Nothing to do on an empty list or
	// if the last element is already smaller than what we look for
	if (list.IsEmpty() || list[list.Size() - 1]->index < index)
	{
		// LOG_DBG("Nothing removed");
		return 0;
	}

	size_t removed = 0;
	for (size_t i = list.Size(); i != 0;)
	{
		--i;
		std::shared_ptr<T> elem = list[i];
		if (elem->index == index || (allFollowing && elem->index > index))
		{
			// LOG_DBG("Removing index={:d}", i);
			list.Erase(i);
			elem.reset();
			++removed;
			if (!allFollowing)
			{
				break;
			}
		}
	}
	return removed;
}

#endif /* SRC_OBJECTMODEL_LISTHELPERS_HPP_ */
