public class Point {
    private int x;
    private int y;

    // 无参构造方法，默认坐标为 (0,0)
    public Point() {
        this.x = 0;
        this.y = 0;
    }

    // 带参构造方法
    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }

    // 获取 x 值
    public int getX() {
        return x;
    }

    // 获取 y 值
    public int getY() {
        return y;
    }

    // 设置 x 值
    public void setX(int x) {
        this.x = x;
    }

    // 设置 y 值
    public void setY(int y) {
        this.y = y;
    }

    // 计算当前点与另一个点 p 之间的距离
    public double distance(Point p) {
        return Math.sqrt((this.x - p.x) * (this.x - p.x) + (this.y - p.y) * (this.y - p.y));
    }
}