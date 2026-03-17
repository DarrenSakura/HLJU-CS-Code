package org.myblog.service.impl;

import org.myblog.entity.User;
import org.myblog.repository.UserRepository;
import org.myblog.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class UserServiceImpl implements UserService {

    @Autowired
    private UserRepository userRepository;

    @Override
    public User findUserByUsername(String username) {
        return userRepository.findUserByUsername(username);
    }

    @Override
    public boolean validateUser(String username, String password) {
        User user = findUserByUsername(username);
        return user != null && user.getPassword().equals(password);
    }
}