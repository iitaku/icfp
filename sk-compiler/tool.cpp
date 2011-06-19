#include <string>
#include <map>
#include "tool.hpp"

namespace copy_kawaii {

typedef std::map<std::string, enum card_code> name_to_card_t;
static name_to_card_t name_to_card_map;

static const char *card_to_name_table[] = {
    "I", "zero", "succ", "dbl", "get", "put",
    "S", "K", "inc", "dec", "attack",
    "help", "copy", "revive", "zombie"
};


const char *
get_card_name(enum card_code cc)
{
    return card_to_name_table[(int)cc];
}

/* 見つからなければCARD_UNKNWONをかえす */
enum card_code
get_card_code(const char* cn)
{
    if (0 == name_to_card_map.size())
    {
        name_to_card_map["I"] = CARD_I;
        name_to_card_map["zero"] = CARD_ZERO;
        name_to_card_map["succ"] = CARD_SUCC;
        name_to_card_map["dbl"] = CARD_DBL;
        name_to_card_map["get"] = CARD_GET;
        name_to_card_map["put"] = CARD_PUT;
        name_to_card_map["S"] = CARD_S;
        name_to_card_map["K"] = CARD_K;
        name_to_card_map["inc"] = CARD_INC;
        name_to_card_map["dec"] = CARD_DEC;
        name_to_card_map["attack"] = CARD_ATTACK;
        name_to_card_map["help"] = CARD_HELP;
        name_to_card_map["copy"] = CARD_COPY;
        name_to_card_map["revive"] = CARD_REVIVE;
        name_to_card_map["zombie"] = CARD_ZOMBIE;
    }

    name_to_card_t::iterator i = name_to_card_map.find(cn);

    if (i == name_to_card_map.end())
    {
        return CARD_I;
    }
    
    return i->second;
}

}
