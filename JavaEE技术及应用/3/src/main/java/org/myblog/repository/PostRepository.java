package org.myblog.repository;

import org.myblog.entity.Post;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.jdbc.core.BeanPropertyRowMapper;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public class PostRepository {

    @Autowired
    private JdbcTemplate jdbcTemplate;

    // 查询所有文章
    public List<Post> findAll() {
        String sql = "SELECT * FROM posts ORDER BY created_at DESC";
        return jdbcTemplate.query(sql, new BeanPropertyRowMapper<>(Post.class));
    }

    // 新增文章
    public int save(Post post) {
        String sql = "INSERT INTO posts (title, content, author_id, category, tags) VALUES (?, ?, ?, ?, ?)";
        return jdbcTemplate.update(sql,
                post.getTitle(),
                post.getContent(),
                post.getAuthorId(),
                post.getCategory(),
                post.getTags());
    }
}