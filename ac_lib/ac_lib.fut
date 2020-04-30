let ac_parity [n][m] (src: [n][m]bool) : [n][m]bool =
    map (\row ->
        map(\col ->
            src[row, (col - 1 + m) %% m] 
                != src[(row - 1 + n) %% n, col]
                != src[row, (col + 1) %% m]
                != src[(row + 1) %% n, col]
        ) (iota m)
    ) (iota n)