package org.myblog.controller;

import org.myblog.entity.Post;
import org.myblog.service.PostService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api/posts")
@CrossOrigin(origins = "*")
public class PostController {

    @Autowired
    private PostService postService;

    // 获取所有文章
    @GetMapping
    public ResponseEntity<List<Post>> getAllPosts() {
        List<Post> posts = postService.findAll();
        return ResponseEntity.ok(posts);
    }

    // 创建新文章
    @PostMapping
    public ResponseEntity<Map<String, Object>> createPost(@RequestBody Post post) {
        int result = postService.save(post);

        Map<String, Object> response = new HashMap<>();
        if (result > 0) {
            response.put("success", true);
            response.put("message", "文章创建成功");
            return ResponseEntity.ok(response);
        } else {
            response.put("success", false);
            response.put("message", "文章创建失败");
            return ResponseEntity.badRequest().body(response);
        }
    }
}