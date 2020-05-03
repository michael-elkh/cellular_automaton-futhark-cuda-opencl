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

entry parity_automaton [n] (src: [n]u32) (width: i32) : [n]u32 =
    unsafe map (\idx ->
        unsafe reduce (^) 0 (unsafe map (\neighbor -> src[neighbor]) (unsafe get_neighborhood idx n width))
    ) (iota n)        

entry cyclic_automaton [n] (src: [n]u32) (width: i32) (max_value: u32) : [n]u32 =
    unsafe map (\idx ->
        let neighbors : [4]u32 = unsafe map (\neighbor -> src[neighbor]) (unsafe get_neighborhood idx n width)

        let k1 : u32 = (src[idx] + 1) % (max_value + 1)
        in  if k1 == neighbors[0]
            then
                neighbors[0]
            else if k1 == neighbors[1]
            then
                neighbors[1]
            else if k1 == neighbors[2]
            then
                neighbors[2]
            else if k1 == neighbors[3]
            then
                neighbors[3]
            else
                src[idx]
    ) (iota n)

entry iterate [n] (parity: bool) (iteration: i32) (src: [n]u32) (width: i32) (max_value: u32) : [n]u32 =
    loop dst = src for i < iteration do
        if parity
        then
            parity_automaton dst width
        else
            cyclic_automaton dst width max_value