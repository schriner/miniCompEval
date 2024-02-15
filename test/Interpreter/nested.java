// This test case ask for interpreting array access.
class array {
    public static void main(String [] args) {
        if ((new Sorter().bubblesortDemo(new int[8])) == true) {
            System.out.println("Sort successful");
        } else {
            System.out.println("Sort failed");
        }
    }
}

class Sorter {
		int [] array;
		int len;
    
		public int printArray() {
			int index;
			System.out.print("Printing Array of Length: ");
			len = array.length;
			System.out.println(len);
			index = 0;
			while (index < len) {
				System.out.print(array[index]);
				System.out.print(" ");
				index = index + 1;
			}
			System.out.println("");
			return 0;
		}
		public boolean bubblesortDemo(int [] a) {
				int res;
        boolean isSorted;
				array = a;
        array[0] = 60;
        array[1] = 40;
        array[2] = 20;
        array[3] = 0;
        array[4] = 50;
        array[5] = 70;
        array[6] = 10;
        array[7] = 30;

				res = this.printArray(array, array.length);

				{
        int i = 0;
        while (i < 8) {
            for (int j = 0; j < 8-i-1; j = j + 1) {
                if (array[j] > array[j+1]) {
                    // swap array[j] and array[j+1]
                    int tmp = array[j+1];
                    array[j+1] = array[j];
                    array[j] = tmp;
                } else {}
            }
            i = i+1;
        }
				}

        // see if the array is already sorted.
        isSorted = true;
        for (int i = 0; i < 8; i = i + 1) {
            isSorted = isSorted && (array[i] == i*10);
            i = i + 1;
        }
				res = this.printArray(array, array.length);
        return isSorted;
    }
}
