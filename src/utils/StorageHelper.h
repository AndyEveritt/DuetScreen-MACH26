#pragma once

#include "Debug.h"
#include "Storage.h"
#include <fstream>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

template <typename T>
concept StorageKeyType = requires(T key) {
	// Identify StorageKey-like types by required members
	{ key.id } -> std::convertible_to<std::string_view>;
	typename T::value_type;
};

class StorageHelper
{
  public:
	template <StorageKeyType T, typename V>
		requires(std::is_convertible_v<V, typename T::value_type>)
	static void setData(const T& key, const V& value)
	{
		ZoneScoped;
		LOG_VERBOSE("Saving \"{:s}\" to config.json", key.id);
		nlohmann::json* current = &data_;

		auto convertor = [](const typename T::value_type& val) -> nlohmann::json
		{
			if constexpr (std::is_convertible<typename T::value_type, nlohmann::json>::value)
			{
				return nlohmann::json(val);
			}
			else
			{
				static_assert(sizeof(typename T::value_type) == 0, "Type T is not convertible to nlohmann::json");
			}
		};

		const nlohmann::json jvalue = convertor(value);

		const std::string_view id = key.id;

		std::size_t pos = 0;
		while (pos <= id.size())
		{
			const std::size_t next = id.find(':', pos);
			const std::string_view segment =
				(next == std::string_view::npos) ? id.substr(pos) : id.substr(pos, next - pos);
			current = &(*current)[segment];
			if (next == std::string_view::npos)
			{
				break;
			}
			pos = next + 1;
		}
		if (*current == jvalue)
		{
			return;
		}
		*current = jvalue;
		save();
	}

	static bool setConfigFile(std::string_view filename);
	static bool save();
	static bool load();
	static bool clear();

	template <StorageKeyType T>
	static typename T::value_type getData(const T& key)
	{
		ZoneScoped;
		nlohmann::json* current = &data_;

		const std::string_view id = key.id;

		std::size_t pos = 0;
		while (pos <= id.size())
		{
			const std::size_t next = id.find(':', pos);
			const std::string_view segment =
				(next == std::string_view::npos) ? id.substr(pos) : id.substr(pos, next - pos);
			// const std::string segmentStr(segment);
			if (!current->contains(segment))
			{
				return detail::resolve_default(key);
			}
			current = &(*current)[segment];
			if (next == std::string_view::npos)
			{
				break;
			}
			pos = next + 1;
		}

		auto convertor = [&key](const nlohmann::json& jval) -> typename T::value_type
		{
			try
			{
				if constexpr (std::is_enum_v<typename T::value_type>)
				{
					const auto val = jval.get<std::underlying_type_t<typename T::value_type>>();
					return magic_enum::enum_cast<typename T::value_type>(val).value_or(
						detail::resolve_default(key));
				}
				else
				{
					return jval.get<typename T::value_type>();
				}
			}
			catch (const nlohmann::json::type_error&)
			{
				LOG_WARN("Config key \"{:s}\" has incompatible type, using default", key.id);
				return detail::resolve_default(key);
			}
		};

		const auto value = convertor(*current);
		return value;
	}

  private:
	inline static nlohmann::json data_;
};
