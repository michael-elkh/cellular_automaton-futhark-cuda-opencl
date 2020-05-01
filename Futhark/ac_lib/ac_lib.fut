let get_neighborhood (idx: i32) (length: i32) (width: i32) : [4]i32 = 
    let col = idx %% width

    let left = if col == 0 
                then 
                    idx + (width - 1) -- Go to the begining of the row
                else 
                    idx - 1

    let up = if idx < width 
                then 
                    length - (width - idx) 
                else 
                    idx - width

    let right = if col == (width - 1) 
                then 
                    idx - (width - 1) 
                else 
                    idx + 1

    let down = if (idx + width) >= length
                then 
                    col -- (idx mod width)
                else 
                    idx + width

    in [left, up, right, down]

let ac_parity [n] (src: [n]u32) (width: i32) : [n]u32 =
    unsafe map (\idx ->
        unsafe reduce (^) 0 (unsafe map (\neighbor -> src[neighbor]) (get_neighborhood idx n width))
    ) (iota n)