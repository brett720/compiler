void main(){
	int a = readIntFromSTDIN();

	int b = 0;
	int result = 0;
	for(b = 1; b <= a; b++){
		result = result + b;
	}	

	printInt(result);
}