#!/usr/bin/ruby

# definition of LTG 
class Slot
    def initialize(vital=100000, field="I")
        @vital = vital
        @field = field
    end

    def show()
        printf("%d, %s\n", @vital, @field)
    end

    def vital()
        @vital
    end

    def field()
        @field
    end
end

class Deck
    def initialize()
        @slots = Array.new(256) { |x| Slot.new }
    end

    def get_slot(slot_index)
        @slots[slot_index]
    end

    def set_slot(slot_index, slot)
        @slots[slot_index] = slot
    end
end

class Table
    def initialize()
        @decks = Array.new(2) { |x| Deck.new }
    end

    def get_slot(player_index, slot_index)
        @decks[player_index].get_slot(slot_index)
    end

    def set_slot(player_index, slot_index, slot)
        @decks[player_index].set_slot(slot_index, slot)
    end
end

# result value -1, 0, 1, respectively means player 0 wins, draw, player 1 wins.
class Game
    def initialize()
        @tables = []
        @alive = [nil, nil]
    end

    def set_alive(index, value)
        @alive[index] = value
    end

    def get_alive(index)
        @alive[index]
    end

    def set_table(table)
        @tables.push(table)
    end

    def get_slot(turn_index, player_index, slot_index)
        @tables[turn_index].get_slot(player_index, slot_index)
    end

    def set_slot(turn_index, player_index, slot_index, slot)
        @tables[turn_index].set_slot(player_index, slot_index, slot)
    end

    def size_turn()
        @tables.size
    end
end

=begin
# repl commands
class NoCmd
    def do(args)
        return

class FileCmd
    def check(args)
        if 1 != len(args)
            raise Exception("Invalid Arg.")
        return   
    def do(args)
        @check(args)
        filename = head(args)
        analyze(filename)
        return

class GameCmd
    global game

    def check(args)
        if 0 != len(args)
            raise Exception("Invalid arg.")
        return
    def do(args)
        @check(args)
        print 'player 0 : %d' % game.alive[0]
        print 'player 1 : %d' % game.alive[1]
        return

class TableCmd
    def check(args)
        return   
    def do(args)
        return

class DeckCmd
    def check(args)
        return   
    def do(args)
        return

=end

class SlotCmd
    def check(args)
        raise "Invalid Arg." if 3 != args.size
    end

    def do(args)
        check(args)
        
        turn = (args.shift.to_i - 1)    # 1 start to 0 start
        player = args.shift.to_i
        index = args.shift.to_i

        slot = $game.get_slot(turn, player, index)

        printf("vitality : %d\n", slot.vital)
        printf("field    : %s\n", slot.field)
    end
end

class SlotTraceCmd
    def check(args)
        raise "Invalid Arg." if 2 != args.size
    end

    def do(args)
        check(args)

        player = args.shift.to_i
        index = args.shift.to_i
        for turn in 0 ... $game.size_turn
            slot = $game.get_slot(turn, player, index)
            printf("%d : {%d,%s}\n", (turn + 1), slot.vital, slot.field)
        end
    end
end

class HelpCmd
    def do(args)
=begin
file path  -- load log file
game       -- print game info
=end
        print <<EOS
slot t p n      -- print slot info about t\'th turn, p\'th player, n\'th slot
slot-trace p n  -- print slot info about all turn p\'th player, n\'th slot
quit            -- quit program
help            -- print help
EOS

    end
end

# global game object
$game = nil

# command list
COMMAND_LIST = { 
=begin
    ''      => NoCmd(),
    'file'  => FileCmd(),
    'game'  => GameCmd(),
    'table' => TableCmd(),
    'deck'  => DeckCmd(),
=end
    'slot'  => SlotCmd.new,
    'slot-trace' => SlotTraceCmd.new,
    'help'  => HelpCmd.new }

def analyze(filename)
    File.open(filename) { |f|
        header = f.gets()
        unless header =~ /^Lambda: The Gathering version/
            raise "Invalid file."
        end

        $game = Game.new
        
        turn_delim = /^###### turn/
        player0_delim = /^player 0 applied/
        player1_delim = /^player 1 applied/
        game_state = /^([0-9]+)=\{(-*[0-9]+),([()a-zA-Z0-9]+)\}$/
        game_result = /!! (won|draw) by [0-9]+:[0-9]+ after turn [0-9]+/

        turn = 0
        player = 0
        table = nil

        while line = f.gets()
            if turn_delim =~ line
                table = Table.new
                player = 0
            elsif player0_delim =~ line
                player = 1
            elsif player1_delim =~ line
                turn += 1
                $game.set_table(table)
            elsif game_state =~ line
                index = $1.to_i
                vital = $2.to_i
                field = $3

                #printf("%d, %d, %d, %d, %s\n", turn, player, index, vital, field)
                table.set_slot(player, index, Slot.new(vital, field))
            elsif game_result =~ line
                score = line.split(' ')[3]
                $game.set_alive(0, score.split(':')[0])
                $game.set_alive(1, score.split(':')[1])
            end
        end
    }
end

def repl()
    print "Lambda: The Gathering -- Log Analyzer\n"

    while true
        printf('>>= ')
        line = gets
        args = line.split(" ")
        command = args.shift
        return if command == 'quit'

        begin
            if obj = COMMAND_LIST[command]
                obj.do(args)
            else
                print "Invalid command.\n"
            end
        rescue => e
            print e, "\n"
        end
    end
end

analyze(ARGV.shift) if 0 < ARGV.size()
repl()
