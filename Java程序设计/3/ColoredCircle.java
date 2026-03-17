package exp3;

import exp2.Circle;
import exp2.Point;

public class ColoredCircle extends Circle {
    private Color borderColor;
    private Color centerColor;

    // 半径赋为0，圆心赋为(0,0)，颜色赋为Color(0,0,0)
    public ColoredCircle() {
        super(new Point(0, 0), 0);
        this.borderColor = new Color();
        this.centerColor = new Color();
    }

    // 半径为radius，圆心为center，颜色为(0,0,0)
    public ColoredCircle(Point center, int radius) {
        super(center, radius);
        this.borderColor = new Color();
        this.centerColor = new Color();
    }

    // 半径为0，圆心为(0,0)，颜色分别为centerColor和borderColor
    public ColoredCircle(Color centerColor, Color borderColor) {
        super(new Point(0, 0), 0);
        this.centerColor = centerColor;
        this.borderColor = borderColor;
    }

    // 圆心为center，半径为radius，圆心颜色为centerColor，边框颜色为borderColor
    public ColoredCircle(Point center, int radius, Color centerColor, Color borderColor) {
        super(center, radius);
        this.centerColor = centerColor;
        this.borderColor = borderColor;
    }

    public void setCenterColor(Color c) { this.centerColor = c; }
    public void setBorderColor(Color c) { this.borderColor = c; }
    public Color getCenterColor() { return centerColor; }
    public Color getBorderColor() { return borderColor; }

    // 方法覆盖：在计算圆的关系前，先输出各自的颜色信息作为扩充
    @Override
    public int relation(Circle c) {
        System.out.println("正在比较两个圆...");
        if (c instanceof ColoredCircle) {
            ColoredCircle cc = (ColoredCircle) c;
            System.out.println("当前圆心颜色: " + this.centerColor + "，目标圆心颜色: " + cc.getCenterColor());
        }
        return super.relation(c);
    }
}