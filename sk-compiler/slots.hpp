#ifndef SLOTS_HPP
#define SLOTS_HPP

/*
 * 
 * 0          : get i の i を入れる
 * 1   - 127  : set_slot で使える
 * 128        : 殺されやすそうなので空けておく
 * 129        : program
 * 130        : imm
 * 131 - 255  : 使ってない
 */

namespace copy_kawaii {

static const int SLOT_IMM = 0;  // IMM は (get i) でとってくる必要があるので先頭に置く
static const int SLOT_PROGRAM = 129;

}

#endif
