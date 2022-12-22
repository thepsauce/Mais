// parse tokens to data points

// examples of input:
// db 0 => (char) 0
// [db] [ 1, 2, 3 ] => (char[]) { 1, 2, 3}
// [[db]] ? => (char**)
// [db] 9 times ? => (char[9])
// [db] [ 2 times (1, 2) ] => (char[]) { 1, 2, 1, 2 }
// [[db]] [ 4 times [ 2 ] ] => { { 2 }, { 2 }, { 2 }, { 2 } }

int parsedata(struct parser *parser, struct datapoint *dp)
{


	return 0;
}
