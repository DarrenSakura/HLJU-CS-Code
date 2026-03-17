package org.myblog.service;

import org.myblog.entity.User;

public interface UserService {
    User findUserByUsername(String username);
    boolean validateUser(String username, String password);
}