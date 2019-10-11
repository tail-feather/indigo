#pragma once
#include <cstring>

template <typename CharArray>
inline void set_char_array(CharArray &dest, const std::string &src) {
    auto size = std::min(sizeof(CharArray)-1, src.size());
    std::memcpy(dest, src.c_str(), size);
    dest[size] = '\0';
}

template <typename C, typename D>
inline auto char_array_getter(D C::*pm) {
    return [pm](C &self) -> std::string {
        return std::string(self.*pm);
    };
}
template <typename C, typename D>
inline auto char_array_setter(D C::*pm) {
    return [pm](C &self, const std::string &value) {
        set_char_array(self.*pm, value);
    };
}
