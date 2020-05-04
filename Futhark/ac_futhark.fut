let get_neighborhood [n] (src: [n]u32) (idx: i32) (width: i32): [4]u32 = 
    let col = idx %% width

    let left = if col == 0 
                then 
                    idx + (width - 1) -- Go to the begining of the row
                else 
                    idx - 1

    let up = if idx < width 
                then 
                    n - (width - idx) 
                else 
                    idx - width

    let right = if col == (width - 1) 
                then 
                    idx - (width - 1) 
                else 
                    idx + 1

    let down = if (idx + width) >= n
                then 
                    col -- (idx mod width)
                else 
                    idx + width

    in map (\x -> src[x]) [left, up, right, down]

let parity_automaton [n] (src: [n]u32) (width: i32) : [n]u32 =
    map (\idx ->
        reduce (\acc neighbor -> acc^neighbor) 0 (get_neighborhood src idx width)
    ) (iota n)        

let cyclic_automaton [n] (src: [n]u32) (width: i32) (max_value: u32) : [n]u32 =
    map (\idx ->
        let k1 : u32 = (src[idx] + 1) % (max_value + 1)
        in if any (\neighbor -> k1==neighbor) (get_neighborhood src idx width)
            then
                k1
            else
                src[idx]
    ) (iota n)

entry iterate [n] (parity: bool) (iteration: i32) (src: [n]u32) (width: i32) (max_value: u32) : [n]u32 =
    loop dst = src for _i < iteration do
        if parity
        then
            parity_automaton dst width
        else
            cyclic_automaton dst width max_value