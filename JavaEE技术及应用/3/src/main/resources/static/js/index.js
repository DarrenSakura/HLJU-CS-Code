document.addEventListener("DOMContentLoaded", async () => {
    // 加载文章列表
    await loadPosts();

    // 加载热门文章
    await loadPopularPosts();
});

// 加载文章列表
async function loadPosts() {
    try {
        const posts = await getAllPosts();
        const postsContainer = document.getElementById('postsContainer');

        if (posts.length === 0) {
            postsContainer.innerHTML = '<p>暂无文章</p>';
            return;
        }

        postsContainer.innerHTML = posts.map(post => `
            <article class="post">
                <h2>${post.title}</h2>
                <p>${post.content}</p>
                <div class="post-meta">
                    <span class="category">${post.category || '未分类'}</span>
                    <span class="date">${formatDate(post.createdAt)}</span>
                </div>
            </article>
        `).join('');
    } catch (error) {
        console.error('加载文章失败:', error);
        document.getElementById('postsContainer').innerHTML = '<p>加载文章失败，请稍后重试</p>';
    }
}

// 加载热门文章
async function loadPopularPosts() {
    try {
        const posts = await getAllPosts();
        const popularPostsContainer = document.getElementById('popularPosts');

        // 简单取前3篇文章作为热门文章
        const popularPosts = posts.slice(0, 3);

        if (popularPosts.length === 0) {
            popularPostsContainer.innerHTML = '<li>暂无热门文章</li>';
            return;
        }

        popularPostsContainer.innerHTML = popularPosts.map(post => `
            <li><a href="#">${post.title}</a></li>
        `).join('');
    } catch (error) {
        console.error('加载热门文章失败:', error);
    }
}

// 搜索文章
async function searchPosts() {
    const searchInput = document.getElementById('searchInput');
    const keyword = searchInput.value.trim();

    if (!keyword) {
        alert('请输入搜索关键词');
        return;
    }

    try {
        const posts = await getAllPosts();
        const filteredPosts = posts.filter(post =>
            post.title.includes(keyword) || post.content.includes(keyword)
        );

        const postsContainer = document.getElementById('postsContainer');

        if (filteredPosts.length === 0) {
            postsContainer.innerHTML = `<p>没有找到包含"${keyword}"的文章</p>`;
            return;
        }

        postsContainer.innerHTML = filteredPosts.map(post => `
            <article class="post">
                <h2>${post.title}</h2>
                <p>${post.content}</p>
                <div class="post-meta">
                    <span class="category">${post.category || '未分类'}</span>
                    <span class="date">${formatDate(post.createdAt)}</span>
                </div>
            </article>
        `).join('');
    } catch (error) {
        console.error('搜索文章失败:', error);
        alert('搜索失败，请稍后重试');
    }
}

// 格式化日期
function formatDate(dateString) {
    const date = new Date(dateString);
    return `${date.getFullYear()}-${(date.getMonth() + 1).toString().padStart(2, '0')}-${date.getDate().toString().padStart(2, '0')}`;
}