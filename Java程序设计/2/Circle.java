public class Circle {
    private Point center;
    private int radius;

    // 无参构造方法，默认圆心(0,0)，半径为1
    public Circle() {
        this.center = new Point();
        this.radius = 1;
    }

    // 使用 x, y 和半径构造
    public Circle(int x, int y, int r) {
        this.center = new Point(x, y);
        this.radius = r;
    }

    // 使用 Point 对象和半径构造
    public Circle(Point c, int r) {
        this.center = c;
        this.radius = r;
    }

    // 计算周长
    public double perimeter() {
        return 2 * Math.PI * radius;
    }

    // 计算面积
    public double area() {
        return Math.PI * radius * radius;
    }

    // 判断当前圆与另一个圆 c 的关系
    // 返回值：0(同一圆)、1(同心圆)、2(相交的圆)、3(分离的圆)、4(包含的圆)
    public int relation(Circle c) {
        double d = this.center.distance(c.center); // 两圆心之间的距离
        
        if (d == 0 && this.radius == c.radius) {
            return 0; // 同一圆：圆心相同且半径相同
        } else if (d == 0 && this.radius != c.radius) {
            return 1; // 同心圆：圆心相同但半径不同
        } else if (d > (this.radius + c.radius)) {
            return 3; // 分离的圆：圆心距大于两半径之和
        } else if (d < Math.abs(this.radius - c.radius)) {
            return 4; // 包含的圆：圆心距小于两半径之差的绝对值
        } else {
            return 2; // 相交的圆（包括内切和外切）：上述情况之外
        }
    }
}