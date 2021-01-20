nc 127.0.0.1 5555 << END
NICK n
USER u 0 * :r
OPER oper oper
join #chan
mode #chan +I chanmask
NICK n1
nick n2
nick n3
quit
END
