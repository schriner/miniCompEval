// This example involves string literal printing, integer printing,
// and if-else branch execution.
// Indicated by the else branch, your interpretation should not print the line in it.
class simple {
    public static void main(String [] args) {
			{
        if (true && (2 < 3)) { // 0
            System.out.println("This line should be printed.");
            System.out.println(32 - 70 - 2 + 9);
            System.out.println((5 + 3) * 17);
        } else {
            System.out.println("This line should not be printed");
        }
				if (3 > 4 || true) { // 1
            System.out.println("This line should be printed: 3 > 4 || true");
				} else {
            System.out.println("This line should not be printed");
				}
				if ((1+2) >= 5 || 5 <= (6*9 - 100)) {	// 2
            System.out.println("This line should not be printed");
				} else {
            System.out.println("This line should be printed: 4 >= 5 || 5 <= (6*9 - 100)");
				}
				if ((0 > 1) && (2 > 3) || false) {		
            System.out.println("This line should not be printed");
				} else {
            System.out.println("This line should be printed: (0 > 1) && (2 > 3) || false");
				}
				if ((0 > 1) && (3 > 2) || false) {		
            System.out.println("This line should not be printed");
				} else {
            System.out.println("This line should be printed: (0 > 1) && (3 > 2) || false");
				}
				if (!!true){
					System.out.println("This line should be printed: !!true");
				} else {
					System.out.println("This line should not be printed");
				}
				if (!!(3 > 5)){
					System.out.println("This line should not be printed");
				} else {
					System.out.println("This line should be printed: !!(3 > 5)");
				}
				if (!(3 > 5)){
					System.out.println("This line should be printed: !(3 > 5)");
				} else {
					System.out.println("This line should not be printed");
				}
			}
    }
}
