package org.myblog.controller;

import org.myblog.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.HashMap;
import java.util.Map;

/**
 * 认证控制器类
 * 处理用户登录相关的HTTP请求
 */
@RestController // 标记该类为RESTful控制器，处理HTTP请求并返回JSON响应
@RequestMapping("/api") // 定义基础URL路径，所有该控制器的请求都以/api开头
@CrossOrigin(origins = "*") // 允许所有来源的跨域请求
public class AuthController { // 认证控制器类，处理用户认证相关功能

    @Autowired // 自动注入UserService实例，实现依赖注入
    private UserService userService; // 用户服务，用于处理用户相关的业务逻辑

    // 用户登录接口
    @PostMapping("/login") // 处理POST请求，映射到/login路径
    public ResponseEntity<Map<String, Object>> login(@RequestBody Map<String, String> loginData) { // 接收请求体中的用户名和密码
        String username = loginData.get("username"); // 从请求体中获取用户名
        String password = loginData.get("password"); // 从请求体中获取密码

        // 检查用户名是否为空
        if (username == null || username.trim().isEmpty()) {
            Map<String, Object> response = new HashMap<>(); // 创建响应对象
            response.put("success", false); // 设置登录失败标志
            response.put("message", "用户名不能为空"); // 设置错误信息
            return ResponseEntity.badRequest().body(response); // 返回400错误响应
        }

        // 检查密码是否为空
        if (password == null || password.trim().isEmpty()) {
            Map<String, Object> response = new HashMap<>(); // 创建响应对象
            response.put("success", false); // 设置登录失败标志
            response.put("message", "密码不能为空"); // 设置错误信息
            return ResponseEntity.badRequest().body(response); // 返回400错误响应
        }

        // 调用用户服务验证用户名和密码
        boolean isValid = userService.validateUser(username, password);

        // 创建响应对象
        Map<String, Object> response = new HashMap<>();
        if (isValid) { // 如果验证成功
            response.put("success", true); // 设置登录成功标志
            response.put("message", "登录成功"); // 设置成功信息
            return ResponseEntity.ok(response); // 返回200成功响应
        } else { // 如果验证失败
            response.put("success", false); // 设置登录失败标志
            response.put("message", "用户名或密码错误"); // 设置错误信息
            return ResponseEntity.badRequest().body(response); // 返回400错误响应
        }
    }
}