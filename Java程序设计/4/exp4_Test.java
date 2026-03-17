package exp4;

public class Test {
    public static void main(String[] args) {
        System.out.println("=== 测试 exp4.Point ===");
        Point p1 = new Point(3, 4);
        Point p2 = new Point(3, 4);
        Point p3 = new Point(5, 6);

        // 测试 toString()
        System.out.println("p1: " + p1.toString());
        System.out.println("p2: " + p2.toString());
        System.out.println("p3: " + p3.toString());

        // 测试 equals()
        System.out.println("p1.equals(p2) (预期为true): " + p1.equals(p2));
        System.out.println("p1.equals(p3) (预期为false): " + p1.equals(p3));

        System.out.println("\n=== 测试 exp4.Circle ===");
        Circle c1 = new Circle(p1, 10);
        Circle c2 = new Circle(3, 4, 10); // 与 c1 圆心和半径相同
        Circle c3 = new Circle(p3, 10);   // 圆心不同，半径相同
        Circle c4 = new Circle(p1, 5);    // 圆心相同，半径不同

        // 测试 toString()
        System.out.println("c1: " + c1.toString());
        System.out.println("c2: " + c2.toString());
        System.out.println("c3: " + c3.toString());
        System.out.println("c4: " + c4.toString());

        // 测试 equals()
        System.out.println("c1.equals(c2) (坐标和半径相同，预期为true): " + c1.equals(c2));
        System.out.println("c1.equals(c3) (坐标不同，预期为false): " + c1.equals(c3));
        System.out.println("c1.equals(c4) (半径不同，预期为false): " + c1.equals(c4));
    }
}