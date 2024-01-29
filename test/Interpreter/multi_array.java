// This test case ask for interpreting array access.
class arrayMulti {
    public static void main(String [] args) {
        if ((new Sorter().bubblesortDemo(new int[7][5])) == true) {
            System.out.println("Sort successful");
        } else {
            System.out.println("Sort failed");
        }
    }
}

class Sorter {
    public int printArray(int [][] array, int len1, int len2) {
			int i;
			int j;
			System.out.print("Printing Array of Dim: ");
			System.out.print(len1);
			System.out.print(" : ");
			System.out.println(len2);
			i = 0;
			j = 0;
			while (i < len1) {
			  System.out.println(": ");
				while (j < len2) {
					System.out.print(array[i][j]);
					j = j + 1;
			  	if (j < len2) { System.out.print(" "); } else {}
				}
				i = i + 1;
			  if (i < len1) { System.out.println(""); } else {}
			}

			return 0;
		}
		public boolean bubblesortDemo(int [][] array) {
        int i;
        int j;
				int res;

				i = 0;
				j = 0;
				while (i < array.length) {
					while (j < array[0].length) {
						array[i][j] = i + j;
						j = j + 1;
					}
					i = i + 1;
				}

				res = this.printArray(array, array.length, array[0].length);

        return false;
    }
}
