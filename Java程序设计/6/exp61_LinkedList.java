package exp61;

public class LinkedList {
    private Node head; // 存储链表第一个节点的引用

    public LinkedList() {
        this.head = null;
    }

    // 方法1：向链表末尾添加元素
    public void add(Object data) {
        Node newNode = new Node(data);
        if (head == null) {
            head = newNode;
        } else {
            Node current = head;
            while (current.getNext() != null) {
                current = current.getNext();
            }
            current.setNext(newNode);
        }
    }

    // 方法2：查找链表中是否存在某个元素（使用 equals 比较）
    public boolean search(Object data) {
        Node current = head;
        while (current != null) {
            // 注意处理 data 为 null 的情况以及使用 equals 比较
            if ((data == null && current.getData() == null) || 
                (data != null && data.equals(current.getData()))) {
                return true;
            }
            current = current.getNext();
        }
        return false;
    }

    // 方法3：删除链表中首次出现的指定元素（使用 equals 比较）
    public boolean remove(Object data) {
        if (head == null) return false;

        // 如果头节点就是要删除的元素
        if ((data == null && head.getData() == null) || 
            (data != null && data.equals(head.getData()))) {
            head = head.getNext();
            return true;
        }

        Node current = head;
        while (current.getNext() != null) {
            Object nextData = current.getNext().getData();
            if ((data == null && nextData == null) || 
                (data != null && data.equals(nextData))) {
                // 跳过被删除的节点
                current.setNext(current.getNext().getNext());
                return true;
            }
            current = current.getNext();
        }
        return false;
    }

    // 方法4：获取链表节点数量
    public int size() {
        int count = 0;
        Node current = head;
        while (current != null) {
            count++;
            current = current.getNext();
        }
        return count;
    }

    // 方法5：判断链表是否为空
    public boolean isEmpty() {
        return head == null;
    }

    // 方法6：清空链表
    public void clear() {
        head = null;
    }

    // 方法7：转换为字符串输出链表内容
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[");
        Node current = head;
        while (current != null) {
            sb.append(current.toString());
            if (current.getNext() != null) {
                sb.append(", ");
            }
            current = current.getNext();
        }
        sb.append("]");
        return sb.toString();
    }
}