let get_neighborhood [n] (src: [n]u32) (idx: i32) (width: i32): (u32, u32, u32, u32) = 
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

    in (src[left], src[up], src[right], src[down])

let parity_automaton [n] (src: [n]u32) (width: i32) : [n]u32 =
    map (\idx ->
        let neighbors = get_neighborhood src idx width
        in neighbors.0 ^ neighbors.1 ^ neighbors.2 ^ neighbors.3
    ) (iota n)        

let cyclic_automaton [n] (src: [n]u32) (width: i32) (max_value: u32) : [n]u32 =
    map (\idx ->
        let k1 : u32 = (src[idx] + 1) % (max_value + 1)
        let neighbors = get_neighborhood src idx width
        in if k1 == neighbors.0 || k1 == neighbors.1 || k1 == neighbors.2 || k1 == neighbors.3
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