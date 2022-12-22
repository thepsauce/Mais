// parse tokens to data points

// examples of input:
// db 0 => (u8) 0
// dw 0 => (u16) 0
// dd 0 => (u32) 0
// dq 0 => (u64) 0
// [db] [ 1, 2, 3 ] => (u8[]) { 1, 2, 3}
// [[db]] ? => (u8**)
// [db] 9 times ? => (u8[9])
// [db] [ 2 times (1, 2) ] => (u8[]) { 1, 2, 1, 2 }
// [[db]] [ 4 times [ 2 ] ] => (u8[][]) { { 2 }, { 2 }, { 2 }, { 2 } }

int parsedata(struct parser *parser, struct datapoint *dp)
{


	return 0;
}
