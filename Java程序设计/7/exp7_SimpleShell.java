package exp7;

import java.io.*;
import java.nio.file.*;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Scanner;

public class SimpleShell {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println("欢迎使用 SimpleShell 文件管理系统！输入 exit 退出。");

        while (true) {
            // 固定提示符
            System.out.print("# ");
            String input = scanner.nextLine().trim();

            if (input.isEmpty()) {
                continue;
            }

            // 按空格分割命令和参数
            String[] parts = input.split("\\s+");
            String command = parts[0];

            try {
                switch (command) {
                    case "exit":
                        System.out.println("退出程序。");
                        scanner.close();
                        System.exit(0);
                        break;
                    case "ls":
                        if (parts.length < 2) {
                            System.out.println("用法: ls <路径>");
                        } else {
                            handleLs(parts[1]);
                        }
                        break;
                    case "cp":
                        if (parts.length < 3) {
                            System.out.println("用法: cp <源文件路径> <目标文件路径>");
                        } else {
                            handleCp(parts[1], parts[2]);
                        }
                        break;
                    case "mv":
                        if (parts.length < 3) {
                            System.out.println("用法: mv <源文件路径> <目标文件路径>");
                        } else {
                            handleMv(parts[1], parts[2]);
                        }
                        break;
                    case "rm":
                        if (parts.length < 2) {
                            System.out.println("用法: rm <文件路径>");
                        } else {
                            handleRm(parts[1]);
                        }
                        break;
                    case "md":
                        if (parts.length < 2) {
                            System.out.println("用法: md <子目录路径>");
                        } else {
                            handleMd(parts[1]);
                        }
                        break;
                    case "rd":
                        if (parts.length < 2) {
                            System.out.println("用法: rd <子目录路径>");
                        } else {
                            handleRd(parts[1]);
                        }
                        break;
                    case "cat":
                        if (parts.length < 2) {
                            System.out.println("用法: cat <文件路径>");
                        } else {
                            handleCat(parts[1]);
                        }
                        break;
                    default:
                        System.out.println("未知命令: " + command);
                }
            } catch (Exception e) {
                System.out.println("执行命令时发生错误: " + e.getMessage());
            }
        }
    }

    // 处理 ls 命令
    private static void handleLs(String pathStr) {
        File dir = new File(pathStr);
        if (!dir.exists() || !dir.isDirectory()) {
            System.out.println("找不到指定的路径或该路径不是一个目录。");
            return;
        }

        File[] files = dir.listFiles();
        if (files == null) {
            System.out.println("无法读取该目录的内容。");
            return;
        }

        SimpleDateFormat sdf = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");

        for (File file : files) {
            String lastModified = sdf.format(new Date(file.lastModified()));
            String typeOrSize = file.isDirectory() ? "<DIR>" : String.valueOf(file.length());
            
            // 使用 printf 格式化输出，保证对齐
            System.out.printf("%s  %-10s %s\n", lastModified, typeOrSize, file.getName());
        }
    }

    // 处理 cp 命令
    private static void handleCp(String src, String dest) throws IOException {
        Path sourcePath = Paths.get(src);
        Path targetPath = Paths.get(dest);
        Files.copy(sourcePath, targetPath, StandardCopyOption.REPLACE_EXISTING);
        System.out.println("复制成功。");
    }

    // 处理 mv 命令
    private static void handleMv(String src, String dest) throws IOException {
        Path sourcePath = Paths.get(src);
        Path targetPath = Paths.get(dest);
        Files.move(sourcePath, targetPath, StandardCopyOption.REPLACE_EXISTING);
        System.out.println("移动成功。");
    }

    // 处理 rm 命令
    private static void handleRm(String filePath) throws IOException {
        Path path = Paths.get(filePath);
        if (Files.isRegularFile(path)) {
            Files.delete(path);
            System.out.println("删除文件成功。");
        } else {
            System.out.println("该路径不是一个文件或文件不存在。");
        }
    }

    // 处理 md 命令 (创建目录)
    private static void handleMd(String dirPath) {
        File dir = new File(dirPath);
        if (dir.mkdirs()) {
            System.out.println("目录创建成功。");
        } else {
            System.out.println("目录创建失败，可能已存在。");
        }
    }

    // 处理 rd 命令 (递归删除非空子目录)
    private static void handleRd(String dirPath) {
        File dir = new File(dirPath);
        if (!dir.exists() || !dir.isDirectory()) {
            System.out.println("指定目录不存在或不是一个目录。");
            return;
        }
        deleteDirectoryRecursively(dir);
        System.out.println("目录删除成功。");
    }

    // 递归删除目录辅助方法
    private static void deleteDirectoryRecursively(File file) {
        if (file.isDirectory()) {
            File[] entries = file.listFiles();
            if (entries != null) {
                for (File entry : entries) {
                    deleteDirectoryRecursively(entry); // 递归
                }
            }
        }
        file.delete();
    }

    // 处理 cat 命令 (显示文本文件内容)
    private static void handleCat(String filePath) {
        File file = new File(filePath);
        if (!file.exists() || !file.isFile()) {
            System.out.println("找不到指定的文件。");
            return;
        }

        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                System.out.println(line);
            }
        } catch (IOException e) {
            System.out.println("读取文件时出错: " + e.getMessage());
        }
    }
}