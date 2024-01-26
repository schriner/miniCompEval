// This example involves string literal printing, integer printing,
// and if-else branch execution.
// Indicated by the else branch, your interpretation should not print the line in it.
class simple {
    public static void main(String [] args) {
        if (true && (2 < 3)) {
            System.out.println("Calling a method:");
            System.out.println( new Factorial().stupidFunction() );
        } else {
            System.out.println("This line should not be printed");
        }
    }
}

class Factorial {
	public int stupidFunction() {
		System.out.println("Function Called");
		System.out.println("Huzzah!");
		return 1*(9*9)*2*67+10*2-39+(10)*8;
	}
}
