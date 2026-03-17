package exp62;

// 实体类：猫 (实现了 Pet 接口)
public class Cat implements Pet {
    private String name;
    private int age;

    public Cat(String name, int age) {
        this.name = name;
        this.age = age;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public int getAge() {
        return age;
    }

    // 重写 equals 方法，以便在链表中根据属性（而不是内存地址）正确删除或查找
    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Cat cat = (Cat) obj;
        return age == cat.age && (name != null ? name.equals(cat.name) : cat.name == null);
    }

    @Override
    public String toString() {
        return "Cat{name='" + name + "', age=" + age + "}";
    }
}