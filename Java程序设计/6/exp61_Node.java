package exp61;

public class Node {
    private Object data; // 用于存放任何类型的数据
    private Node next;   // 保存下一个节点的引用

    // 构造方法
    public Node(Object data) {
        this.data = data;
        this.next = null;
    }

    // getter 和 setter
    public Object getData() {
        return data;
    }

    public void setData(Object data) {
        this.data = data;
    }

    public Node getNext() {
        return next;
    }

    public void setNext(Node next) {
        this.next = next;
    }

    // 重写 toString
    @Override
    public String toString() {
        return data == null ? "null" : data.toString();
    }
}