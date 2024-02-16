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

// This test case tests while loop execution.
class PrintFactorial10 extends Factorial{
	boolean b;
	public boolean printFact() {
		f = 10;
		while (f != 0) {
			System.out.print("Factorial of ");
			System.out.print(this.fact());
			System.out.print(" is:");
			System.out.println(this.computeDec());
		}
		return this.fact() == 0;
	}
}
