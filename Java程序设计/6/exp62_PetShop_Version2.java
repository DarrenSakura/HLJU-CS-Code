package exp62;

import exp61.LinkedList;

public class PetShop {
    private LinkedList pets;

    public PetShop() {
        this.pets = new LinkedList();
    }

    // 1. 将宠物上架
    public void add(Pet pet) {
        if (pet != null) {
            pets.add(pet);
        }
    }

    // 2. 将宠物下架
    public boolean delete(Pet pet) {
        if (pet != null) {
            return pets.remove(pet);
        }
        return false;
    }

    // 3. 查找第 index 个宠物
    public Pet get(int index) {
        Object obj = pets.get(index);
        if (obj instanceof Pet) {
            return (Pet) obj;
        }
        return null;
    }

    // 4. 返回宠物总数
    public int size() {
        return pets.size();
    }
}