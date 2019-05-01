import os

BLOCK_LEVELS = [
"#                  #\n"+
"#                  #\n"+
"#                  #\n"+
"#   #       #      #\n"+
"#D  #   # B # B U  #\n"+
"####################",

" #    ##        ##    \n"+
" #                #   \n"+
"##                 #  \n"+
"#D                  # \n"+
"##                   #\n"+
" #           #  B    #\n"+
" #           #B BBU  #\n"+
" #####   #############\n"+
"     #  B#            \n"+
"     #####            ",

" #                 \n"+
" #   ############# \n"+
"# # #             #\n"+
"#  #              #\n"+
"#                B#\n"+
"#               BB#\n"+
"# ###    U   #B ## \n"+
"# # #    #  #####  \n"+
"# # #BB ##  #      \n"+
"#D# ###### ##      \n"+
"### ##   ###       ",

"                  #     \n"+
"                 # #    \n"+
"       #        #   #   \n"+
"      # #      #     #  \n"+
"   ###   #    #       # \n"+
"  #       #  #         #\n"+
" #         ##          #\n"+
" #                    B#\n"+
" #                   BB#\n"+
" #               U   ###\n"+
"##    #          #   #  \n"+
"#D    # B        #####  \n"+
"##### # B   B  ###      \n"+
"    # # B # #B #        \n"+
"    # ##########        \n"+
"    ###                 ",

"     ###    ######### \n"+
" ####   ####         #\n"+
"#                    #\n"+
"#                    #\n"+
"#                    #\n"+
"#     #              #\n"+
"#     #              #\n"+
"#     #BBBB          #\n"+
"#D   #######U        #\n"+
"## ###     ## #     B#\n"+
" # #        # ##   BB#\n"+
" # #        # ##  BBB#\n"+
" ###        # ########\n"+
"            ###       ",

" ###             ####\n"+
" #  #############   #\n"+
"##                  #\n"+
"#D                  #\n"+
"##                  #\n"+
" #                BB#\n"+
" #BB        #  B  ###\n"+
" #BBB       #UBBB #  \n"+
" #BBBB      ##### #  \n"+
" #####    ###   ###  \n"+
"     #   B#          \n"+
"     ## ###          \n"+
"      ###            ",

"  #   #####   ##   ###  \n"+
" # # #     # #  # #   # \n"+
" #  ##      ##   ##    #\n"+
" #   #       #    #    #\n"+
" #                    B#\n"+
" #                    B#\n"+
"##                   BB#\n"+
"#D   B               ###\n"+
"##   # B     #    ## #  \n"+
" #   # B    ## B U####  \n"+
" ##  # BBB  ## BBB#     \n"+
"  #  ###### #######     \n"+
"  ## #    ###           \n"+
"   ###                  ",

" ###       ####   #######  \n"+
"#   #     #    # #       # \n"+
"#    #   #     ##         #\n"+
"#B    ###    # #     ###  #\n"+
"#BB         ##      ## #  #\n"+
"####       ##          #D #\n"+
"   ##            ##    ## #\n"+
"  #    B #      #  #      #\n"+
"  #    B# #    #   #      #\n"+
" #   ###   #    #  #     B#\n"+
" #      # #      ##     BB#\n"+
"#        #           ######\n"+
"#            B            #\n"+
"#    B      ###          B#\n"+
"#   ###                 BB#\n"+
"#        B       B  U  BBB#\n"+
"###########################",

"        ###         \n"+
"       #   #        \n"+
"      #     #  #####\n"+
"     #       ##    #\n"+
"    #     B        #\n"+
"   #      BB      B#\n"+
"  #       ###    BB#\n"+
" #            U ####\n"+
"#             B    #\n"+
"#D           ###   #\n"+
"##    ##   #      B#\n"+
" #    ##B  ##   ####\n"+
" #    #######  ##   \n"+
" ###  #     # ##    \n"+
"   # ##     ###     \n"+
"   ###              ",

"   #####################   \n"+
" ##           #         #  \n"+
"####B       BB#B   BBB B## \n"+
"#  ##  #   #####  B### ## #\n"+
"#   #  ##        ### ###  #\n"+
"#   ##  ##BBBB            #\n"+
"#D       #######          #\n"+
"##        #   ###        ##\n"+
" #     B   # #  ##        #\n"+
" #     #    #    ##       #\n"+
" ####  ##             #####\n"+
"   #####      U           #\n"+
"   #          #           #\n"+
"   #         ##    ########\n"+
"   #        ##           # \n"+
"   #          B         B# \n"+
"   #B    ###########   BB# \n"+
"   #BB  ##         ## BBB# \n"+
"   ######           ###### ",

"#############################\n"+
"#  #   #                    #\n"+
"#     B#BB            ##### #\n"+
"#B   ### B##     B  ##  D # #\n"+
"#BB    ###   U  B       # # #\n"+
"###  BB#     # B          # #\n"+
"#   ####      #  ###   ###  #\n"+
"#B            # #      #  B #\n"+
"#BB       ### # #B    #  ####\n"+
"#### B   ###  # ##B  # B #  #\n"+
"#           B ###  B#   #   #\n"+
"#   B     BB #   ####       #\n"+
"#    #########        ##### #\n"+
"#              B   B##    # #\n"+
"####           B   #    BB# #\n"+
"#B##   #    #          #### #\n"+
"##B### #    #   BBB B       #\n"+
"#B#B#B##    #        BBB    #\n"+
"#############################"]

replacement = {
    ' ': 'VOID',
    '#': 'BRICK',
    'B': 'BOX',
    'D': 'DOOR',
    'U': 'GUY'
}

fileTemplate = """#include \"../main.h\"
#include \"../level.h\"

const static unsigned char compressedLevel[] = ^{data}~;

const LevelData level{n} = ^
    .data = compressedLevel,
    .width = {width},
    .height = {height},
~;"""

#{BRICK, 1, VOID, 18, BRICK, 2, VOID, 18, BRICK, 2, VOID, 3, BRICK, 1, VOID, 7, BRICK, 1, VOID, 6, BRICK, 2, DOOR, 1, VOID, 2, BRICK, 1, VOID, 3, BRICK, 1, VOID, 1, BOX, 1, VOID, 1, BRICK, 1, VOID, 1, BOX, 1, VOID, 4, BRICK, 21, VOID, 0};


def rlc(level):
    char = level[0]
    count = 1
    result = []
    for i in range(1, len(level)):
        if(level[i] == char):
            count += 1
        else:
            result.append(replacement[char])
            result.append(count)
            char = level[i]
            count = 1

    result.append(replacement[char])
    result.append(count)
    return result

def generateFileContent(rlcData, n, width, height):
    tileArray = ""
    i = 0
    while i < len(rlcData) - 2:
        tileArray += "{tile}, {count}, ".format(tile=rlcData[i], count=rlcData[i+1])
        i += 2
    
    tileArray += "{tile}, {count}".format(tile=rlcData[-2], count=rlcData[-1])
    return fileTemplate.format(n=n, width=width, height=height, data=tileArray).replace("^", "{").replace("~", "}")

def writeToFile(n, data): 
    dir = 'inc/generated'
    if(not os.path.isdir(dir)):
        os.mkdir(dir)
    
    path = 'inc/generated/{n}.h'.format(n=n)
    file = open(path, 'w')
    file.write(data)

def generate(level, n):
    width = level.find("\n")
    level = level.replace("\n", "")
    height = int(len(level) / width)
    rlcData = rlc(level)
    fileContent = generateFileContent(rlcData, n, width, height)
    writeToFile(n, fileContent)
    
generate(BLOCK_LEVELS[0], 1)

