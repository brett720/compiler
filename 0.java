int sum(int a, int b, int c, int d, int e){
	int result = a + b + c + d + e;
	return result;
}

void main(){
	int aa = readIntFromSTDIN();
	int bb = readIntFromSTDIN();
	int cc = readIntFromSTDIN();
	int dd = readIntFromSTDIN();
	int ee = readIntFromSTDIN();

	int res = sum(aa, bb, cc, dd, ee);

	printInt(res);
}