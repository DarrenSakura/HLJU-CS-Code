package org.myblog.service;

import org.myblog.entity.Post;

import java.util.List;

/**
 * PostService接口定义了与帖子相关的业务操作方法
 * 该接口提供了帖子的基本增查功能
 */
public interface PostService {
    /**
     * 查询所有帖子的方法
     * @return 返回一个包含所有帖子的列表
     */
    List<Post> findAll(); // 查询系统中所有帖子的方法，返回Post对象的列表
    /**
     * 保存新帖子的方法
     * @param post 要保存的帖子对象
     * @return 返回保存操作影响的行数，通常为1表示成功
     */
    int save(Post post); // 将新的帖子信息保存到数据库中，返回受影响的行数
}