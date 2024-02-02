// You are supposed to support method calling for testing this case.
// The minimum required would be calling a method with an empty parameter list and belongs to a class without instance variables.
class loop {
    public static void main(String [] args) {
        {
				/*System.out.print("Divide 7/3:");
				System.out.println(7/3);
				System.out.print("Divide 6/3:");
				System.out.println(6/3);
				System.out.print("Divide 0/3:");
				System.out.println(0/3);
				System.out.print("Divide 2/3:");
				System.out.println(2/3);
				System.out.print("Divide -2/3:");
				System.out.println(-2/3);
				System.out.print("Divide -6/3:");
				System.out.println(-6/3);
				System.out.print("Divide -7/3:");
				System.out.println(-7/3);
        */
				System.out.print("Factorial of 10 is:");
        System.out.println(new Factorial().compute10());
        }
    }
}

// This test case tests while loop execution.
class Factorial {
    public int compute10() {
        int prod;
        int multiplier;
				boolean b;
        multiplier = 1;
        prod = 1;
        //System.out.print("multiplier:");
        //System.out.println(multiplier);
        //System.out.print("prod:");
        //System.out.println(prod);
        while (multiplier <= 10) {
            prod = prod * multiplier;
            multiplier = multiplier + 1;
        }
        return prod;
    }
}
