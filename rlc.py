level = "4                  44                  44   4       4      442  4   4 3 4 3    444444444444444444444"
char = level[0]
count = 1
result = []
for i in range(1, len(level)):
    if(level[i] == char):
        count += 1
    else:
        if(char == ' '):
            result.append(0)
        else:
            result.append(ord(char) - ord('1'))
        
        result.append(count)
        char = level[i]
        count = 1

if(char == ' '):
    result.append(0)
else:
    result.append(ord(char) - ord('1'))

result.append(count)
print(result)