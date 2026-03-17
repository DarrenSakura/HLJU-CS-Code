package exp5;

import java.util.Arrays;

public class ArrayReverser {
    private int[] data;

    public ArrayReverser(int[] data) {
        this.data = data;
    }

    public int[] getData() {
        return data;
    }

    public void setData(int[] data) {
        this.data = data;
    }

    public void reverse() {
        if (data == null || data.length <= 1) {
            return;
        }
        int left = 0;
        int right = data.length - 1;
        while (left < right) {
            int temp = data[left];
            data[left] = data[right];
            data[right] = temp;
            left++;
            right--;
        }
    }

    @Override
    public String toString() {
        return Arrays.toString(data);
    }
}