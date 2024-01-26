// You are supposed to support method calling for testing this case.
// The minimum required would be calling a method with an empty parameter list and belongs to a class without instance variables.
class loop {
    public static void main(String [] args) {
        {
        System.out.println(new Factorial().compute10());
        System.out.println(new Factorial().compute10());
        }
    }
}

// This test case tests while loop execution.
class Factorial {
	boolean a;
	boolean b;
	public int compute10() {
				a = !(!true == true) || false;
				System.out.println(a);
				a = (true == true) || false;
				return a;
				System.out.println(a);
				b = (false == true) && true;
        return b;
    }
}
