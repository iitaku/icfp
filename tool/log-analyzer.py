#!/usr/bin/python

import sys
import re
   
class Slot:
    def __init__(self, vital=100000, field="I"):
        self.vital = vital
        self.field = field
        return

class Deck:
    def __init__(self, slots=[Slot]*255):
        self.slots = slots
        return

class Table:
    def __init__(self, pro_decks=Deck, opp_decks=Deck):
        self.pro_decks = pro_decks
        self.opp_decks = opp_decks
        return

class Game:
    def __init__(self, tables=[Table]):
        self.tables = tables
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
        return   
    def do(self, args):
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
        return   
    def do(self, args):
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
        result = re.search('^Lambda: The Gathering version', header)
        
        if None == result:
            raise Exception("Invalid file.")

        game = Game
        
        turn_delim = re.compile('^#+ turn')
        
        game_state = re.compile('^[0-9]+=\{-*[0-9]+,[()a-zA-Z0-9]+\}$')
        
        for line in f:
            #result = turn_delim.search(line)
            #if None == result:
            #    continue
            
            result = game_state.search(line)
            if None != line:
                print line
    return
    
def repl():
    print "Lambda: The Gathering -- Log Analyzer"

    while True:
        line = raw_input('=>> ')
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
     
