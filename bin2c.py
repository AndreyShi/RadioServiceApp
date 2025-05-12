import sys
# sys.argv[0] - name this file
# sys.argv[1] - input bin file (*.txt)
# sys.argv[2] - output file (*.h)
in_file_name = []
out_file_name = "output.h"
cnt = 0
if len(sys.argv) >= 2:
    in_file_obj  = open(sys.argv[1])
    out_file_obj = open(out_file_name,"w")
    out_file_obj.write('const char output[] = {\n')
    while 1:
        l = in_file_obj.read(1)
        if l == '':
            print("find EOF break")
            break
        elif l == ' ':
            continue
        elif l == '\n':
            continue
        else:
            ascii_byte = "0x" + l + in_file_obj.read(1) + ', '
            out_file_obj.write(ascii_byte)
            cnt = cnt + 1
            if cnt >= 15:
                out_file_obj.write('\n')
                cnt = 0
    out_file_obj.write('\n};')
else:
    print("ERROR: need input file name as argument")

