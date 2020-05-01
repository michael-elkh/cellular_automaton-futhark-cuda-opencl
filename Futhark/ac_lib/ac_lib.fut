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
    let top = height - 1
    let right = width - 1
    in unsafe map (\idx ->
        let row = idx // width
        let col = idx %% width
        
        let up_row = if row == 0 then 0 else row - 1
        let down_row = if row == top then top else row + 1
        let left_col = if col == 0 then 0 else col - 1
        let right_col = if col == right then right else col + 1

        in src[row*width + left_col] ^ src[up_row * width + col] ^ src[row * width + right_col] ^ src[down_row * width + col]
    ) (iota n)