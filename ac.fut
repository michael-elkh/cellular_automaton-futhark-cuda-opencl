module ac_lib = import "ac_lib/ac_lib"

entry ac_parity [n][m] (src: [n][m]bool) : [n][m]bool = ac_lib.ac_parity src

let main (n:i32) : bool = 
    let src = iota (100000*100000)
    let ac = ac_parity (unflatten 100000 100000 (map bool.i32 src))
    in reduce (!=) false (flatten ac)