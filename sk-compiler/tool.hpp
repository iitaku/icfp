#ifndef TOOL_HPP
#define TOOL_HPP

namespace copy_kawaii {

enum card_code {
    CARD_I,
    CARD_ZERO,
    CARD_SUCC,
    CARD_DBL,
    CARD_GET,
    CARD_PUT,
    CARD_S,
    CARD_K,
    CARD_INC,
    CARD_DEC,
    CARD_ATTACK,
    CARD_HELP,
    CARD_COPY,
    CARD_REVIVE,
    CARD_ZOMBIE,
    CARD_UNKNOWN
};

const char *get_card_name(enum card_code cc);
enum card_code get_card_code(const char* cn);

} /* namespace copy_kawaii */

#endif
