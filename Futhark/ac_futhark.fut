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

let parity_automaton [n] (src: [n]u32) (width: i32) : [n]u32 =
    map (\idx ->
        reduce (^) 0 (map (\neighbor -> src[neighbor]) (get_neighborhood idx n width))
    ) (iota n)        

let cyclic_automaton [n] (src: [n]u32) (width: i32) (max_value: u32) : [n]u32 =
    map (\idx ->
        let k1 : u32 = (src[idx] + 1) % (max_value + 1)
        in  if any (k1==) (map (\neighbor -> src[neighbor]) (get_neighborhood idx n width))
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