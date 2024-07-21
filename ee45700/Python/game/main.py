import os
import getpass
import re

# TODO: change this to your ltspice install directory
ltspice_location = "C:/Program\ Files/LTC/LTspiceXVII/"


def tobin(d):
    # ASCII definitions go here
    return {
        '0': "00110000",
        '1': "00110001",
        '2': "00110010",
        '3': "00110011",
        '4': "00110100",
        '5': "00110101",
        '6': "00110110",
        '7': "00110111",
        '8': "00111000",
        '9': "00111001"
    }[d]


def getval(b):
    return {
        '0': "0",
        '1': "3.3"
    }[b]

# Get the input string from player 1
player_1_input = ""
while True:
    player_1_input = getpass.getpass('Enter 4 digits(Do not repeat digits|0-9): \n')
    player_1_length = len(player_1_input)
    player_1_set_length = len(set(player_1_input))
    if player_1_length == 4 and player_1_set_length == 4:
        break

key_a = tobin(player_1_input[0])
key_b = tobin(player_1_input[1])
key_c = tobin(player_1_input[2])
key_d = tobin(player_1_input[3])

# initial net list for circuit
old_file = open('../../enigma4.spi', 'r')
old_listing = old_file.readlines()
old_file.close()

temp_listing = list()
for line in old_listing:
    line1 = line
    if "VKEY_A" in line:
        bit = int(line[13]) - 1
        line1 = re.sub("DC ([0-9]*.+)", "DC " + getval(key_a[bit]), line)
    if "VKEY_B" in line:
        bit = int(line[13]) - 1
        line1 = re.sub("DC ([0-9]*.+)", "DC " + getval(key_b[bit]), line)
    if "VKEY_C" in line:
        bit = int(line[13]) - 1
        line1 = re.sub("DC ([0-9]*.+)", "DC " + getval(key_c[bit]), line)
    if "VKEY_D" in line:
        bit = int(line[13]) - 1
        line1 = re.sub("DC ([0-9]*.+)", "DC " + getval(key_d[bit]), line)
    temp_listing.append(line1)

tries = 0
while tries < 10:
    new_listing = list()
    player_2_input = input('Try to guess the 4 digit sequence(Do not repeat digits|0-9): \n')
    player_2_length = len(player_2_input)
    player_2_set_length = len(set(player_2_input))
    if player_2_length == 4 and player_2_set_length == 4:
        guess_a = tobin(player_2_input[0])
        guess_b = tobin(player_2_input[1])
        guess_c = tobin(player_2_input[2])
        guess_d = tobin(player_2_input[3])
        for line in temp_listing:
            line2 = line
            if "VGUESS_A" in line:
                bit = int(line[17]) - 1
                line2 = re.sub("DC ([0-9]*.+)", "DC " + getval(guess_a[bit]), line)
            if "VGUESS_B" in line:
                bit = int(line[17]) - 1
                line2 = re.sub("DC ([0-9]*.+)", "DC " + getval(guess_b[bit]), line)
            if "VGUESS_C" in line:
                bit = int(line[17]) - 1
                line2 = re.sub("DC ([0-9]*.+)", "DC " + getval(guess_c[bit]), line)
            if "VGUESS_D" in line:
                bit = int(line[17]) - 1
                line2 = re.sub("DC ([0-9]*.+)", "DC " + getval(guess_d[bit]), line)
            new_listing.append(line2)
        #os.chdir(ltspice_location)
        new_file = open('new_listing.spi', 'w')
        for item in new_listing:
            new_file.write(item)
        new_file.close()
        # TODO: remove the 'wine ./' from the command below
        ltspice_win_cmd = ltspice_location + "XVIIx64.exe -Run -b new_listing.spi"
        os.system(ltspice_win_cmd)
        new_log = open('new_listing.log', 'r')
        items = ["place_1", "place_2", "place_3", "place_4", "match_1", "match_2", "match_3", "match_4"]
        values = list()
        for line in new_log:
            for item in items:
                line = line.replace("\0", "")
                if item in line:
                    val = re.findall(r"=([0-9]*.+)", line)
                    if '3.3' in val:
                        values.append(1)
                    else:
                        values.append(0)
        places = 0
        matches = 0
        for i, val in enumerate(values):
            if i < 4 and val is 1:
                places += 1
            elif i >= 4 and val is 1:
                matches += 1
        if places == 4:
            print("You guessed the correct sequence " + player_1_input)
            break
        else:
            for x in range(0, places):
                print(u'\u25CF', end="")
            for x in range(0, matches):
                print(u'\u25CD', end="")
            rest = 4 - places - matches
            for x in range(0, rest):
                print(u'\u25CB', end="")
        tries += 1
        print("\nNumber of guesses left:", 10 - tries)
