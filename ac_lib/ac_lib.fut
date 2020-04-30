let ac_parity [n][m] (src: [n][m]u32) : [n][m]u32 =
    map (\row ->
        map(\col ->
            src[row, (col - 1 + m) %% m] 
                ^ src[(row - 1 + n) %% n, col]
                ^ src[row, (col + 1) %% m]
                ^ src[(row + 1) %% n, col]
        ) (iota m)
    ) (iota n)

let ac_parity_1d [n] (src: [n]u32) (width: i32) (height: i32) : [n]u32 =
    map (\idx ->
        let row = idx // width
        let col = idx % width
        in src[row*width + (col - 1 + width) %% width] 
                ^ src[((row - 1 + height) %% height) * width + col]
                ^ src[row * width + (col + 1) %% width]
                ^ src[((row + 1) %% height) * width + col]
    ) (iota n)