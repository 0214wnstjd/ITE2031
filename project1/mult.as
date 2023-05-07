        lw      0       2       mcand   
        lw      0       3       mplier
        lw      0       4       mask
        lw      0       6       time
start   nor     3       4       5
        beq     0       5       mul
next    lw      0       7       neg1
        add     7       6       6
        beq     0       6       done
        add     2       2       2
        add     4       4       4
        lw      0       7       pos1
        add     4       7       4
        beq     0       0       start
mul     add     1       2       1
        beq     0       0       next
done    halt
mcand   .fill   32766
mplier  .fill   12328
mask    .fill   -2
time    .fill   15
neg1    .fill   -1
pos1    .fill   1
