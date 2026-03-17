package exp4;

public class Point extends exp2.Point {

    // 新的无参构造方法，调用父类无参构造
    public Point() {
        super();
    }

    // 新的带参构造方法，调用父类带参构造
    public Point(int x, int y) {
        super(x, y);
    }

    // 重写 toString 方法
    @Override
    public String toString() {
        return "Point(" + getX() + ", " + getY() + ")";
    }

    // 重写 equals 方法，判断两个点坐标是否完全一致
    @Override
    public boolean equals(Object obj) {
        // 1. 判断是否是同一个对象的引用
        if (this == obj) {
            return true;
        }
        // 2. 判断 obj 是否为 null，或者两者是否类型不一致
        if (obj == null || getClass() != obj.getClass()) {
            return false;
        }
        // 3. 强转并比较属性值
        Point point = (Point) obj;
        return this.getX() == point.getX() && this.getY() == point.getY();
    }
}