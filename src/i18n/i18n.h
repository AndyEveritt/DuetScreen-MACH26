/*
 * LvI18n.h
 *
 *  Created on: 2025-10-13
 *      Author: Andy Everitt
 */

#pragma once

#include <cstdint>
#include <string_view>
#include <vector>
#include <map>

namespace i18n
{
    /* Load translation files from `assets/i18n` */
    void init();

    std::string_view getCurrentLanguage();
    void refreshLanguageFiles();
    const std::map<std::string, std::string>& getAvailableLanguages();
    int32_t getLanguageIndex(std::string_view lang);
    
    bool setLanguage(const std::string_view lang);
    bool setLanguageByIndex(size_t index);

    const std::string& translate(const std::string& tag);
} // namespace i18n

static inline const std::string& _(const std::string& text) { return i18n::translate(text); }
