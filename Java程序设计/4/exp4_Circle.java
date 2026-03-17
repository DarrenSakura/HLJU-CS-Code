package exp4;

import exp2.Point;

public class Circle extends exp2.Circle {

    // 无参构造方法
    public Circle() {
        super();
    }

    // 使用坐标和半径构造
    public Circle(int x, int y, int r) {
        super(x, y, r);
    }

    // 使用 Point 对象和半径构造
    public Circle(Point c, int r) {
        super(c, r);
    }

    // 重写 toString 方法
    @Override
    public String toString() {
        Point c = getCenter();
        return "Circle[Center=(" + c.getX() + ", " + c.getY() + "), Radius=" + getRadius() + "]";
    }

    // 重写 equals 方法，判断两个圆的圆心坐标和半径是否一致
    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null || getClass() != obj.getClass()) {
            return false;
        }
        Circle circle = (Circle) obj;
        
        // 比较半径
        if (this.getRadius() != circle.getRadius()) {
            return false;
        }
        
        // 比较圆心坐标
        Point c1 = this.getCenter();
        Point c2 = circle.getCenter();
        if (c1 == null && c2 == null) return true;
        if (c1 == null || c2 == null) return false;
        
        return c1.getX() == c2.getX() && c1.getY() == c2.getY();
    }
}