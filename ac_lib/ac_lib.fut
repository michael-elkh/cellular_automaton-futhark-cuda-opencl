let ac_parity [n][m] (src: [n][m]bool) : [n][m]bool =
    unflatten n m (
        map (\idx1d ->
            let index2d = (idx1d // m,idx1d %% m)
            in src[index2d.0,  (index2d.1 - 1 + m) %% m] 
                != src[(index2d.0 - 1 + n) %% n,  index2d.1]
                != src[index2d.0,  (index2d.1 + 1) %% m]
                != src[(index2d.0 + 1) %% n,  index2d.1]
        )
        (iota (n*m))
    )