package exp5;

import java.util.Random;

public class ArrayTest {
    public static void main(String[] args) {
        int[] arr = new int[10];
        Random random = new Random();
        for (int i = 0; i < arr.length; i++) {
            arr[i] = random.nextInt(100); // 填充0-99的随机数
        }

        ArrayReverser reverser = new ArrayReverser(arr);
        System.out.println("反转前的数据: " + reverser.toString());
        
        reverser.reverse();
        
        System.out.println("反转后的数据: " + reverser.toString());
    }
}