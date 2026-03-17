package org.myblog.service.impl;

import org.myblog.entity.Post;
import org.myblog.repository.PostRepository;
import org.myblog.service.PostService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * 帖子服务实现类
 * 实现了PostService接口，提供帖子相关的业务逻辑处理
 */
@Service
public class PostServiceImpl implements PostService {

    /**
     * 帖子数据访问层，用于与数据库交互
     */
    @Autowired
    private PostRepository postRepository;

    /**
     * 查询所有帖子
     * @return 返回所有帖子的列表
     */
    @Override
    public List<Post> findAll() {
        return postRepository.findAll();
    }

    /**
     * 保存帖子信息
     * @param post 要保存的帖子对象
     * @return 返回保存操作的结果（通常为受影响的行数）
     */
    @Override
    public int save(Post post) {
        return postRepository.save(post);
    }
}