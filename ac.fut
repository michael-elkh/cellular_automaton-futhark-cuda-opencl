module ac_lib = import "ac_lib/ac_lib"

entry ac_parity [n][m] (src: [n][m]u32) : [n][m]u32 = ac_lib.ac_parity src
entry ac_parity_1d [n] (src: [n]u32) (width: i32) (height: i32) : [n]u32 = ac_lib.ac_parity_1d src width height