class loop2 {
    public static void main(String [] args) {
        {
				if (new PrintFactorial10().printFact()) {
        	System.out.println("Success");
				} else {
					System.out.println("Failure");
				}
        }
    }
}
class PrintFactorial10 {
	Factorial f;
	boolean b;
	public boolean printFact() {
		f = new Factorial();
		b = f.factorial(10);
		while (f.fact() != 0) {
			System.out.print("Factorial of ");
			System.out.print(f.fact());
			System.out.print(" is:");
			System.out.println(f.computeDec());
		}
		return f.fact() == 0;
	}
}
class Factorial {
		int f;
		public boolean factorial(int fact) {
			f = fact;
			return f == fact;
		}
		public int fact() {
			return f;
		}
    public int computeDec() {
        int prod;
        int multiplier;
        multiplier = 1;
        prod = 1;
        while (multiplier <= f) {
            prod = prod * multiplier;
            multiplier = multiplier + 1;
        }
				f = f - 1;
        return prod;
    }
}
