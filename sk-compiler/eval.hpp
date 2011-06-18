#ifndef EVAL_HPP
#define EVAL_HPP

#include "expr.hpp"
#include "command.hpp"
#include <map>
#include <string>

namespace copy_kawaii {


/* slot 上に expr や 文字列を評価した結果を出力する
 * コマンド{lr,slot,card}列を生成する
 *
 * このコマンドを write_line に入れると
 *  1. サーバーに命令を送る
 *  2. シミュレータの状態を更新する
 *
 */

/*
 * あとやること 
 *  - hook入れたい
 *  - robust にする … スロットが死んでるとき、殺されたときの対応
 *
 * スロットが何とかなった時の対応を入れる可能性があるので、
 * slot は整数じゃなくなるかも!!!
 */

/*
 * 今は slot 0 を定数生成に使っているので使わないでね
 */

/*
 * expr にはいくらかの変換が適用される
 *  - 整数の展開
 *  - SK への変換
 *
 * 途中結果見たいときは、中のdumpをtrueにする
 */
void eval_at(commands &coms,int slot, expr *expr, var_map_t &vm);

void eval_at(commands &coms,
             int slot,
             const char *prog,
             var_map_t &vm);

void eval_and_run_at(int slot, const char *prog, var_map_t &vm);

/* 文字列 -> expr やりたいときは parse_expr を使う */


/* expr を解放する方法は…無い! 諦めろ! */
}

#endif
