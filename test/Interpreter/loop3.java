// You are supposed to support method calling for testing this case.
// The minimum required would be calling a method with an empty parameter list and belongs to a class without instance variables.
class loop {
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

// This test case tests while loop execution.
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
        prod = 1;
				//System.out.print(prod);
				//System.out.print(" ");
        for (int multiplier = 1; multiplier <= f; multiplier = multiplier + 1) {
						//System.out.println(multiplier);
            prod = prod * multiplier;
						//System.out.print(prod);
						//System.out.print(" ");
						//System.out.println(multiplier);
        }
				f = f - 1;
        return prod;
    }
}
