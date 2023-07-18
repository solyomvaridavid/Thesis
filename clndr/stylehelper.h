#pragma once

#include <QString>

class StyleHelper
{
public:
    StyleHelper() = delete;
    StyleHelper(const StyleHelper&) = delete;
    StyleHelper(StyleHelper&&) = delete;

    StyleHelper& operator=(const StyleHelper&) = delete;
    StyleHelper& operator=(StyleHelper&&) = delete;

    static QString globalStyle;
    static QString text;
};
