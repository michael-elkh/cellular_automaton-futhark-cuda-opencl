module ac_lib = import "ac_lib/ac_lib"

entry ac_parity [n] (src: [n]u32) (width: i32) (height: i32) : [n]u32 = unsafe ac_lib.ac_parity src width height