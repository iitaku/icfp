#!/usr/bin/python

import sys
import re

# definition of LTG 
class Slot:
    def __init__(self, vital=100000, field="I"):
        self.vital = vital
        self.field = field
        return

class Deck:
    def __init__(self, slots=[Slot()]*256):
        self.slots = slots
        return

class Table:
    def __init__(self, decks=[Deck()]*2):
        self.decks = decks
        return

# result value -1, 0, 1, respectively means player 0 wins, draw, player 1 wins.
class Game:
    def __init__(self, tables=[Table()]):
        self.tables = tables
        self.alive = [None, None]
        return

# repl commands
class NoCmd:
    def do(self, args):
        return

class FileCmd:
    def check(self, args):
        if 1 != len(args):
            raise Exception("Invalid Arg.")
        return   
    def do(self, args):
        self.check(args)
        filename = head(args)
        analyze(filename)
        return

class GameCmd:
    def check(self, args):
        if 0 != len(args):
            raise Exception("Invalid arg.")
        return
    def do(self, args):
        self.check(args)
        print 'player 0 : %d' % game.alive[0]
        print 'player 1 : %d' % game.alive[1]
        return

class TableCmd:
    def check(self, args):
        return   
    def do(self, args):
        return

class DeckCmd:
    def check(self, args):
        return   
    def do(self, args):
        return

class SlotCmd:
    def check(self, args):
        if 3 != len(args):
            raise Exception("Invalid Arg.")
        return
    def do(self, args):
        self.check(args)
        
        turn = int(take(args))
        player = int(take(args))
        index = int(take(args))
        
        slot = game.tables[turn].decks[player].slots[index]
        
        print 'vitality : %d' % slot.vital
        print 'field    : %s' % slot.field
        return

class HelpCmd:
    def check(self, args):
        return   
    def do(self, args):
        return

# global game object
game = None

# command list
command_list = { 
    ''      : NoCmd(),
    'file'  : FileCmd(),
    'game'  : GameCmd(),
    'table' : TableCmd(),
    'deck'  : DeckCmd(),
    'slot'  : SlotCmd(),
    'help'  : HelpCmd(),
}

def head(list):
    return list[0]

def take(list):
    ret = list[0]
    del list[0]
    return ret

def analyze(filename):
    global game

    with open(filename) as f:

        header = f.readline()
        result = re.match('^Lambda: The Gathering version', header)
        
        if None == result:
            raise Exception("Invalid file.")

        game = Game()
        
        turn_delim = re.compile('^###### turn')
        player_delim = re.compile('^\*\*\* player [0-1]\'s turn, with slots:$')
        game_state = re.compile('^[0-9]+=\{-*[0-9]+,[()a-zA-Z0-9]+\}$')
        game_result = re.compile('!! won|draw by [0-9]+:[0-9]+ after turn [0-9]+')
        
        turn = 0
        player = 1
        
        for line in f:
            result = turn_delim.match(line)
            if None != result:
                game.tables.append(Table())
                turn += 1
                continue

            result = player_delim.match(line)
            if None != result:
                player ^= 1
                continue

            result = game_state.match(line)
            if None != result:
                state = result.group()
                state = state.split('=')
                index = int(state[0])
                state = state[1].strip('{}').split(',')
                vital = int(state[0])
                field = state[1]
                
                game.tables[turn].decks[player].slots[index] = Slot(vital, field)
                continue
        
            result = game_result.match(line)
            if None != result:
                game_result = result.group().split(' ')
                game.alive[0] = game_result[3].split(':')[0]
                game.alive[1] = game_result[3].split(':')[1]
    return
    
def repl():
    print "Lambda: The Gathering -- Log Analyzer"

    while True:
        line = raw_input('>>= ')
        args = line.split(" ")
        command = take(args)
               
        try:
            if command in command_list:
                command_list[command].do(args)
            else:
                print "Invalid command."
        
        except Exception as e:
            print e
    return
        
def main():
        
    if 1 < len(sys.argv):
        analyze(sys.argv[1])
   
    repl()

    return

if '__main__' == __name__:
    main()       
     
