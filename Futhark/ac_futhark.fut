import "lib/github.com/athas/vector/vector"
module vec2 = cat_vector vector_1 vector_1
module vec4 = cat_vector vec2 vec2

let get_neighborhood [n][m] (src: [n][m]u32) (row:i32) (col:i32) : vec4.vector u32 = 
    let left = if col == 0
                then 
                    m - 1
                else 
                    col - 1

    let up = if row == 0
                then
                    n - 1
                else
                    row - 1

    let right = if col == (m-1)
                then 
                    0
                else 
                    col + 1

    let down = if row == (n-1)
                then 
                    0
                else 
                    row + 1

    in vec4.from_array ([src[row, left], src[up, col], src[row, right], src[down, col]] :> [vec4.length]u32)

let parity_automaton [n][m] (src: [n][m]u32) : [n][m]u32 =
    map (\row ->
        map(\col -> 
            vec4.reduce (^) 0 (get_neighborhood src row col)
        ) (iota m)
    ) (iota n)

let cyclic_automaton [n][m] (src: [n][m]u32) (max_value: u32) : [n][m]u32 =
    map (\row ->
        map(\col ->
            let k1 : u32 = (src[row, col] + 1) % (max_value + 1)
            in if vec4.reduce (||) false (vec4.map (k1==) (get_neighborhood src row col))
                then
                    k1
                else
                    src[row, col]
        ) (iota m)
    ) (iota n)

entry iterate [n][m] (parity: bool) (iteration: i32) (src: [n][m]u32) (max_value: u32) : [n][m]u32 =
    loop dst = src for _i < iteration do
        if parity
        then
            parity_automaton dst
        else
            cyclic_automaton dst max_value