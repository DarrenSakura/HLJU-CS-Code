package exp62;

public class Test {
    public static void main(String[] args) {
        // 1. 创建宠物商店
        PetShop shop = new PetShop();

        // 2. 创建几只宠物
        Cat cat1 = new Cat("小白", 2);
        Cat cat2 = new Cat("小黑", 3);
        Cat cat3 = new Cat("大橘", 1);

        // 3. 测试上架 (add)
        shop.add(cat1);
        shop.add(cat2);
        shop.add(cat3);
        System.out.println("成功上架3只宠物，当前宠物总数: " + shop.size());

        // 4. 测试获取宠物 (get)
        Pet p = shop.get(1); // 索引从0开始，1应该是小黑
        if (p != null) {
            System.out.println("获取第1只宠物: " + p.getName() + "，年龄: " + p.getAge());
        }

        // 5. 测试下架 (delete)
        // 我们创建一个与 cat2 属性完全相同的对象去下架，验证链表的 equals 是否起效
        Cat targetToRemove = new Cat("小黑", 3);
        boolean isDeleted = shop.delete(targetToRemove);
        System.out.println("下架名叫'小黑'的宠物是否成功: " + isDeleted);
        System.out.println("下架后当前宠物总数: " + shop.size());

        // 打印剩余宠物
        for (int i = 0; i < shop.size(); i++) {
            System.out.println("剩余宠物[" + i + "]: " + shop.get(i).toString());
        }
    }
}