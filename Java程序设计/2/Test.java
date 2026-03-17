public class Test {
    public static void main(String[] args) {
        // --- 1. 测试 Point 类 ---
        Point p1 = new Point(0, 0);
        Point p2 = new Point(3, 4);
        System.out.println("点 p1 的坐标: (" + p1.getX() + ", " + p1.getY() + ")");
        System.out.println("点 p2 的坐标: (" + p2.getX() + ", " + p2.getY() + ")");
        System.out.println("p1 和 p2 之间的距离: " + p1.distance(p2)); // 勾股定理，应为5.0
        System.out.println("--------------------------------------------------");

        // --- 2. 测试 Circle 类 ---
        Circle c1 = new Circle(0, 0, 5);
        Circle c2 = new Circle(p1, 5);    // 同一圆（与 c1）
        Circle c3 = new Circle(0, 0, 10); // 同心圆（与 c1）
        Circle c4 = new Circle(10, 0, 5); // 相交的圆（与 c1 刚好外切，按相交算）或分离测试
        Circle c5 = new Circle(20, 20, 2); // 分离的圆（与 c1）
        Circle c6 = new Circle(1, 0, 2);  // 包含的圆（在 c1 内部）

        System.out.println("圆 c1 的周长: " + c1.perimeter());
        System.out.println("圆 c1 的面积: " + c1.area());
        System.out.println("--------------------------------------------------");

        // --- 3. 测试圆之间的关系输出 ---
        checkRelation(c1, c2, "c1 和 c2");
        checkRelation(c1, c3, "c1 和 c3");
        checkRelation(c1, c4, "c1 和 c4");
        checkRelation(c1, c5, "c1 和 c5");
        checkRelation(c1, c6, "c1 和 c6");
    }

    // 辅助方法：调用 relation 方法并根据返回值输出对应字符串
    public static void checkRelation(Circle circle1, Circle circle2, String names) {
        int status = circle1.relation(circle2);
        System.out.print(names + " 的关系是: ");
        switch (status) {
            case 0:
                System.out.println("同一圆");
                break;
            case 1:
                System.out.println("同心圆");
                break;
            case 2:
                System.out.println("相交的圆");
                break;
            case 3:
                System.out.println("分离的圆");
                break;
            case 4:
                System.out.println("包含的圆");
                break;
            default:
                System.out.println("未知状态");
                break;
        }
    }
}