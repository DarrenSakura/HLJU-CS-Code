// API基础URL
const API_BASE_URL = 'http://localhost:8080/api';

// 通用API请求函数
/**
 * 发起API请求的异步函数
 * @param {string} endpoint - API端点路径
 * @param {object} options - 请求配置选项，默认为空对象
 * @returns {Promise<any>} 返回API响应的JSON数据
 * @throws {Error} 当请求失败时抛出错误
 */
async function apiRequest(endpoint, options = {}) {
    try {
        // 发起fetch请求，拼接基础URL和端点
        const response = await fetch(`${API_BASE_URL}${endpoint}`, {
            // 设置请求头，合并默认的Content-Type和传入的headers
            headers: {
                'Content-Type': 'application/json',
                ...options.headers
            },
            // 合并默认的请求选项和传入的options
            ...options
        });

        // 检查响应状态是否成功
        if (!response.ok) {
            // 如果响应不成功，抛出包含状态码的错误
            throw new Error(`HTTP error! status: ${response.status}`);
        }

        // 解析并返回响应的JSON数据
        return await response.json();
    } catch (error) {
        // 捕获并打印错误，然后重新抛出
        console.error('API请求失败:', error);
        throw error;
    }
}

// 获取所有文章
async function getAllPosts() {
    return await apiRequest('/posts');
}

// 用户登录
async function login(username, password) {
    return await apiRequest('/login', {
        method: 'POST',
        body: JSON.stringify({ username, password })
    });
}

// 创建新文章
async function createPost(postData) {
    return await apiRequest('/posts', {
        method: 'POST',
        body: JSON.stringify(postData)
    });
}