import java.util.Scanner;

public class FactorialTest {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.print("请输入一个整数 N (0 <= N <= 20): ");
        
        if (scanner.hasNextInt()) {
            int n = scanner.nextInt();
            
            if (n < 0 || n > 20) {
                System.out.println("错误：N 的取值应在 0 到 20 之间！");
            } else {
                long factorial = 1; // 使用 long 类型防止数据溢出
                for (int i = 1; i <= n; i++) {
                    factorial *= i;
                }
                System.out.println(n + " 的阶乘是: " + factorial);
            }
        } else {
            System.out.println("错误：请输入一个有效的整数！");
        }
        
        scanner.close();
    }
}