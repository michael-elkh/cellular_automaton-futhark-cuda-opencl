import "lib/github.com/athas/vector/vector"
module vec2 = cat_vector vector_1 vector_1
module vec4 = cat_vector vec2 vec2

let get_neighborhood [n] (src: [n]u32) (idx: i32) (width: i32) : vec4.vector u32 = 
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

    in vec4.from_array ([src[left], src[up], src[right], src[down]] :> [vec4.length]u32)

let parity_automaton [n] (src: [n]u32) (width: i32) : [n]u32 =
    map (\idx ->
        vec4.reduce (^) 0 (get_neighborhood src idx width)
    ) (iota n)        

let cyclic_automaton [n] (src: [n]u32) (width: i32) (max_value: u32) : [n]u32 =
    map (\idx ->
        let k1 : u32 = (src[idx] + 1) % (max_value + 1)
        in if vec4.reduce (||) false (vec4.map (k1==) (get_neighborhood src idx width))
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