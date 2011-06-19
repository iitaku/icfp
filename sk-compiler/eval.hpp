#ifndef EVAL_HPP
#define EVAL_HPP

#include "expr.hpp"
#include "command.hpp"
#include "translators.hpp"
#include "events.hpp"
#include <map>
#include <string>

namespace copy_kawaii {

typedef std::map<std::string, bool> critical_slots_t;

struct CriticalHandler {
    bool type_error_critical;

    critical_slots_t critical_slots;

    /* リカバリできたら true を返す
     * コマンドを発行するときは
     * 相手の状態をコマンドを読み終わった状態で返すこと
     *
     * critical_event が複数ある場合に、
     * 一部だけ処理した場合、それらをcritical_eventから消しておくこと
     */
    virtual bool recovery(event_list_t &ciritcal_event) { return false; }

    CriticalHandler()
        :type_error_critical(true)
    {
    }

    void add_critical_slot(std::string const &slot) {
        critical_slots[slot] = true;
    }

    void remove_critical_slot(std::string const &slot) {
        critical_slots.erase(slot);
    }
};

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
 *   -> いれた by iitaku
 *      const char*を引数にとるeval_*_atの中で、hookに登録された条件式を実行し、0以外が帰ったらhookプログラムを実行する。
 *      条件式には、event_listとhookプログラムのexprとvar_mapが渡り、後者２つはいじれる。（必要ならproとoppをみる）
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
void eval_at(commands &coms, expr *expr, var_map_t &vm,
             CompileParam const &cp);

void eval_at(commands &coms,
             const char *prog,
             var_map_t &vm,
             CompileParam const &cp);

/* prog を実行し、完了するかもしくは
 * クリティカルなイベントが発生したら返る
 */
event_list_t eval_and_run_at(expr *e,
                             var_map_t &vm,
                             CompileParam const &cp,
                             CriticalHandler &ch);

event_list_t eval_and_run_at(const char *prog,
                             var_map_t &vm,
                             CompileParam const &cp,
                             CriticalHandler &ch);

/* 文字列 -> expr やりたいときは parse_expr を使う */


/* expr を解放する方法は…無い! 諦めろ! */
}

#endif
