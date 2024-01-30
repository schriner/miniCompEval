// This test case ask for interpreting array access.
class arrayMulti {
    public static void main(String [] args) {
        if ((new Sorter().bubble(new int[7][5])) == 10) {
            System.out.println("Array successful");
        } else {
            System.out.println("Array failed");
        }
    }
}

class Sorter {
    public int printArray(int [][] array) {
			int i;
			int j;
			System.out.print("Printing Array of Dim: ");
			System.out.print(array.length);
			System.out.print(" : ");
			System.out.println(array[0].length);
			i = 0;
			while (i < array.length) {
			  System.out.println(": ");
			  j = 0;
				while (j < array[0].length) {
					System.out.print(array[i][j]);
					j = j + 1;
			  	if (j < len2) { System.out.print(" "); } else {}
				}
				i = i + 1;
			  if (i < len1) { System.out.println(""); } else {}
			}

			return 0;
		}
		public int bubble(int [][] array) {
        int i;
        int j;
				int res;

				i = 0;
				while (i < array.length) {
			    j = 0;
					while (j < array[0].length) {
						array[i][j] = i + j;
						j = j + 1;
					}
					i = i + 1;
				}

				res = this.printArray(array, array.length, array[0].length);

        return array[array.length - 1][array[0].length - 1];
    }
}
